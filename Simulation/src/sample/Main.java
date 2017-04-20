package sample;

import com.sun.javafx.geom.Vec2f;
import domains.GridMap;
import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.print.*;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.Pane;
import javafx.stage.Stage;
import javafx.util.Duration;
import search.FlexibleAStar;
import domains.GridMapParser;


public class Main extends Application
{
    private static int CANVAS_MAX_SIZE = 5000;

    private Vec2f mouseClicked = new Vec2f();
    private boolean hasClicked = false;

    private Stage stage;
    private Group centerGroup;
    private Scene scene;
    private GraphicsContext tileGC;
    private GraphicsContext agentGC;
    private GraphicsContext pathGC;

    private static Simulation simulation;
    private GridMap map;
    private FlexibleAStar aStar;

    @Override
    public void start(Stage stage) throws Exception
    {

        new GridMapParser("maps/newMap.map");
//        Parent centerGroup = FXMLLoader.load(getClass().getResource("sample.fxml"));
        double width = 1280;
        double height = 720;

        Canvas tileCanvas = new Canvas(CANVAS_MAX_SIZE, CANVAS_MAX_SIZE);
        tileGC = tileCanvas.getGraphicsContext2D();

        Canvas agentCanvas = new Canvas(CANVAS_MAX_SIZE, CANVAS_MAX_SIZE);
        agentGC = agentCanvas.getGraphicsContext2D();

        Canvas pathCanvas = new Canvas(CANVAS_MAX_SIZE, CANVAS_MAX_SIZE);
        pathGC = agentCanvas.getGraphicsContext2D();

        // sample.Main node where rendering occurs
        centerGroup = new Group();
        centerGroup.setOnMouseDragged(this::OnDragged);
        centerGroup.setOnMouseReleased(e -> hasClicked = false);

        Pane layeredPane = new Pane();
        layeredPane.getChildren().add(tileCanvas);
        layeredPane.getChildren().add(agentCanvas);
        layeredPane.getChildren().add(pathCanvas);
        agentCanvas.toFront();
        pathCanvas.toFront();

//        layeredPane.setOnMouseDragged(this::OnDragged);
//        layeredPane.setOnMouseReleased(e -> hasClicked = false);

        centerGroup.getChildren().add(layeredPane);

        BorderPane pane = new BorderPane(centerGroup);
        pane.setStyle("-fx-background-color: #000000;");

        // Scene is the window
        scene = new Scene(pane, width, height);
        this.stage = stage;
        stage.setTitle("Warehouse Automation");
        stage.setScene(scene);
        stage.sizeToScene();
        stage.show();

        scene.setOnKeyPressed(this::OnKeyPressed);
        scene.setOnKeyReleased(this::OnKeyReleased);
        scene.setOnScroll(this::OnScrolled);

        // 144 fps
        Timeline timeline = new Timeline(new KeyFrame(Duration.millis(7), this::OnTick));
        timeline.setCycleCount(Animation.INDEFINITE);
        timeline.play();

        // Testing
        simulation = new Simulation();

//        simulation.getMap().getTiles().forEach(t -> centerGroup.getChildren().add(t));
//        UpdateVisibleTiles();
//        simulation.getAgents().forEach(a -> centerGroup.getChildren().add(a));

        simulation.drawTiles(tileGC);

    }

    private void OnTick(ActionEvent e)
    {
        simulation.tick(0.016f);
    }

    private void OnKeyPressed(KeyEvent e)
    {
        if (e.getCode() == KeyCode.ENTER)
        {
            simulation.step();

            agentGC.clearRect(0, 0, CANVAS_MAX_SIZE, CANVAS_MAX_SIZE);
            simulation.drawAgents(agentGC);

            simulation.drawPaths(pathGC);


//            UpdateSearch();
        }
        else if (e.getCode() == KeyCode.Q)
        {
//            tickSearch = true;
        }
        else if (e.getCode() == KeyCode.P)
        {
            PrintCenterStage();
        }
    }

    private void PrintCenterStage()
    {
        Printer pdfPrinter = null;
        for (Printer printer : Printer.getAllPrinters())
            if (printer.getName().endsWith("PDF"))
                pdfPrinter = printer;

        assert pdfPrinter != null;
        PrinterJob job = PrinterJob.createPrinterJob(pdfPrinter);

        PageLayout layout = pdfPrinter.createPageLayout(Paper.A3, PageOrientation.LANDSCAPE, Printer.MarginType.HARDWARE_MINIMUM);

        job.getJobSettings().setPageLayout(layout);
        job.getJobSettings().setJobName("Sample Printing Job");

        job.showPrintDialog(stage); // Window must be your main Stage
        job.printPage(centerGroup);

        job.endJob();
    }


    private void OnKeyReleased(KeyEvent e)
    {
        if (e.getCode() == KeyCode.Q)
        {
//            tickSearch = false;
        }
    }

    private void OnScrolled(ScrollEvent e)
    {
        double scaleDelta = 1 + Math.signum(e.getDeltaY()) * 0.05f;

        Tile.GRID_SIZE = Tile.GRID_SIZE * scaleDelta;
        System.out.println(Tile.GRID_SIZE);

        tileGC.clearRect(0, 0, CANVAS_MAX_SIZE, CANVAS_MAX_SIZE);
        simulation.drawTiles(tileGC);
        agentGC.clearRect(0, 0, CANVAS_MAX_SIZE, CANVAS_MAX_SIZE);
        simulation.drawAgents(agentGC);
        simulation.drawPaths(pathGC);

//        centerGroup.setScaleX(centerGroup.getScaleX() * scaleDelta);
//        centerGroup.setScaleY(centerGroup.getScaleY() * scaleDelta);
    }

    private void OnDragged(MouseEvent e)
    {
//        System.out.println("Dragging");
        if (hasClicked)
        {

            centerGroup.setTranslateX(centerGroup.getTranslateX() + (e.getX() - mouseClicked.x));
            centerGroup.setTranslateY(centerGroup.getTranslateY() + (e.getY() - mouseClicked.y));

            UpdateVisibleTiles();
        }
        else
        {
            mouseClicked.set((float) e.getX(), (float) e.getY());
            hasClicked = true;
        }
    }

    private void UpdateVisibleTiles()
    {
        Tile[][] tiles = simulation.getMap().getTilesXY();

        double xS = -centerGroup.getTranslateX();
        double yS = -centerGroup.getTranslateY();

        double xE = xS + scene.getWidth() * 1.25f;
        double yE = yS + scene.getHeight() * 1.25f;

//        centerGroup.getChildren().clear();

        int numberOfTilesRenderered = 0;

//        System.out.println("");

//        for (int x = (int) (xS / Tile.GRID_SIZE); x < (int) (xE / Tile.GRID_SIZE); x++)
//        {
//            for (int y = (int) (yS / Tile.GRID_SIZE); y < (int) (yE / Tile.GRID_SIZE); y++)
//            {
//                if (x < 0 || y < 0 || x > simulation.getMap().width - 1 || y > simulation.getMap().height - 1) continue;

//                if (tiles[x][y] != null)
//                {
//                    numberOfTilesRenderered++;
//                    centerGroup.getChildren().add(tiles[x][y]);
//                }
//            }
//        }
//        System.out.println("Rendering " + numberOfTilesRenderered + " tiles of " + simulation.getMap().width * simulation.getMap().height);
    }

    public static Simulation getSimulation()
    {
        return simulation;
    }

    public static void main(String[] args)
    {
        launch(args);
    }
}
