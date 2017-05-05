package sample;

import domains.GridMap;
import domains.GridMapParser;
import expanders.GridMapExpansionPolicy;
import heuristics.ManhattanHeuristic;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import search.FlexibleAStar;
import search.ProblemInstance;
import utils.ReservationTable;
import warehouse.DriveUnit;
import warehouse.PickingStation;
import warehouse.StoragePod;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;

public class Simulation
{
    public GridMap map;
    public FlexibleAStar aStar;
    public List<Agent> agents;
    public List<StoragePod> storagePods;
    public List<PickingStation> pickingStations;

    public ReservationTable reservationTable;
    public int timestep;


    public Simulation()
    {
        agents = new ArrayList<>();
        storagePods = new ArrayList<>();
        pickingStations = new ArrayList<>();
    }

    public void init(String mapPath)
    {
        ProblemInstance instance = new ProblemInstance("instances/warehouse1.instance");
        GridMapParser mapParser = new GridMapParser(instance);

        map = new GridMap(mapParser);
        aStar = new FlexibleAStar<>(new ManhattanHeuristic(mapParser.getMetaInfo()), new GridMapExpansionPolicy(map));

        // Add actors to simulation
        for (int i = 0; i < 1; i++)
            agents.add(new DriveUnit(map.getRandomNode(), aStar));

        reservationTable = new ReservationTable();
    }

    public void step()
    {
//        System.out.println("\t### Timestep " + timestep + " ###");
        for (Agent agent : agents)
        {
            agent.step();
//            reservationTable.update(timestep, agent);
        }

//        reservationTable.findConflicts(timestep);
//        System.out.println(timestep);
//        reservationTable.print();


//        System.out.println(reservationTable.table.size());

        agents.forEach(a -> a.tick(0));

        timestep += 1;
    }

    void tick(float dt)
    {
        agents.forEach(a -> a.tick(dt));
    }

    void drawAgentIds(GraphicsContext gc)
    {
        for (Agent agent : agents)
        {
            gc.setStroke(Color.BLACK);
            gc.strokeText(
                    "" + agent.agentId,
                    agent.circle.getCenterX(),
                    agent.circle.getCenterY());
        }
    }

    void drawTiles(GraphicsContext gc)
    {
        for (Tile tile : map.getTiles())
        {
            gc.setFill(tile.getFill());
            gc.fillRect(
                    tile.tilePos.getX() * Tile.GRID_SIZE,
                    tile.tilePos.getY() * Tile.GRID_SIZE,
                    Tile.GRID_SIZE * 0.95,
                    Tile.GRID_SIZE * 0.95);
//
//            gc.setStroke(Color.TEAL);
//            gc.strokeText(
//                    "" + tile.tilePos.getX() + "," + tile.tilePos.getY(),
//                    tile.tilePos.getX() * Tile.GRID_SIZE,
//                    tile.tilePos.getY() * Tile.GRID_SIZE);
        }
    }

    public void drawCollisions(GraphicsContext gc)
    {
        gc.setFill(Color.RED);
        for (List<Point> positionList : reservationTable.collisions.values())
        {
            for (Point collision : positionList)
            {
                gc.fillOval(
                        collision.x * Tile.GRID_SIZE + Tile.GRID_SIZE / 2 - Tile.GRID_SIZE * 0.125,
                        collision.y * Tile.GRID_SIZE + Tile.GRID_SIZE / 2 - Tile.GRID_SIZE * 0.125,
                        Tile.GRID_SIZE * 0.25,
                        Tile.GRID_SIZE * 0.25);
            }
        }
    }

    public void drawPaths(GraphicsContext gc)
    {
//        gc.setGlobalAlpha(0.1);
//        gc.setStroke(Color.BLACK);
        gc.setLineWidth(2f);

        Point start;
        Point end;

        gc.setStroke(Color.BLACK);

        for (Agent agent : agents)
        {
//            gc.setStroke(agent.color);

            for (int i = 1; i < agent.path.size() + 1; i++)
            {
                start = agent.path.get(i - 1).getTile().tilePos;

                if (i == agent.path.size())
                    end = agent.currentNode.getTile().tilePos;
                else
                    end = agent.path.get(i).getTile().tilePos;

                gc.strokeLine(
                        start.x * Tile.GRID_SIZE + Tile.GRID_SIZE / 2,
                        start.y * Tile.GRID_SIZE + Tile.GRID_SIZE / 2,
                        end.x * Tile.GRID_SIZE + Tile.GRID_SIZE / 2,
                        end.y * Tile.GRID_SIZE + Tile.GRID_SIZE / 2);
            }
        }
    }

    void drawStoragePods(GraphicsContext gc)
    {
        gc.setLineWidth(0);
        gc.setFill(Color.GREEN);
        for (StoragePod pod : storagePods)
        {
            gc.fillRect(
                    pod.homeNode.x * Tile.GRID_SIZE,
                    pod.homeNode.y * Tile.GRID_SIZE,
                    Tile.GRID_SIZE * 0.8,
                    Tile.GRID_SIZE * 0.8);
        }
    }

    void drawPickingStations(GraphicsContext gc)
    {
        gc.setLineWidth(0);
        gc.setFill(Color.RED);
        for (PickingStation station : pickingStations)
        {
            gc.fillRect(
                    station.node.x * Tile.GRID_SIZE,
                    station.node.y * Tile.GRID_SIZE,
                    Tile.GRID_SIZE * 0.8,
                    Tile.GRID_SIZE * 0.8);
        }
    }
}
