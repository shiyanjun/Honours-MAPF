package sample;

import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import search.SearchNode;

import java.awt.*;

public class Tile extends Rectangle
{
    public enum TileType
    {
        EMPTY, BLOCKED, START, GOAL
    }

    public static double GRID_SIZE = 8;
    public Point tilePos;
    public double centerX;
    public double centerY;
    public TileType tileType;
    public SearchNode node;

    public Tile(int x, int y, TileType tileType)
    {
        super(x * (GRID_SIZE + 1), y * (GRID_SIZE + 1), GRID_SIZE, GRID_SIZE);

        SetType(tileType);
        tilePos = new Point(x, y);

//        setStroke(javafx.scene.paint.Color.BLACK);
//        setStrokeWidth(.5f);

//        setX(x * (GRID_SIZE + 1));
//        setY(y * (GRID_SIZE + 1));

        centerX = getX() + GRID_SIZE / 2;
        centerY = getY() + GRID_SIZE / 2;

        setOnMouseClicked(this::OnMouseClicked);
    }

    public void setSearchNode(SearchNode node)
    {
        this.node = node;
    }

    public void SetType(TileType newType)
    {
//        if (tileType == TileType.START)
//            Main.getSimulation().setStart(node.getX(), node.getY());
//
//        if (tileType == TileType.GOAL)
//            Main.getSimulation().setEnd(node.getX(), node.getY());

        switch (newType)
        {
            case EMPTY:

                setFill(Color.WHITE);
                break;
            case BLOCKED:
                setFill(Color.GREY);
                break;
            case START:
                sample.Main.getSimulation().setStart(node.getX(), node.getY());
                setFill(Color.GREEN);
                break;
            case GOAL:
                sample.Main.getSimulation().setEnd(node.getX(), node.getY());
                setFill(Color.RED);
                break;
        }

        tileType = newType;
    }

    private void OnMouseClicked(MouseEvent e)
    {
        switch (tileType)
        {
            case EMPTY:
                switch (e.getButton())
                {
                    case PRIMARY:
                        if (node != null) SetType(TileType.START);

                        break;
                    case SECONDARY:
                        if (node != null) SetType(TileType.GOAL);
                }
                break;
            case START:
                SetType(TileType.EMPTY);
                break;
            case GOAL:
                SetType(TileType.EMPTY);
                break;
            case BLOCKED:
                break;
        }
    }
}