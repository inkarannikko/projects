#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <vector>
#include <deque>
#include <random>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QRect>
#include <QMessageBox>
/* Match 3
 *
 * Kuvaus:
 * Ohjelma toteuttaa Match 3 fruit flip pelin, jossa pyritään hedelmien paikkoja vaihtamalla saamaan samanvärisiä hedelmiä vierekkäin.
 * Pelilauta on kooltaan ROWS x COLUMNS. Aluksi pelilaudalle arvotaan hedelmät niin, että matcheja ei vielä ole muodostunut.
 * Kahden vierekkäisen hedelmän paikkaa voi yrittää vaihtaa klikkaamalla niit ja vaihto tedhään jos sen seurauksena tulee match.
 * Jos pelilaudalla on kolme tai useampi hedelmä peräkkäin ne poistetaan ja jokaisesta poistetusta hedelmästä saa yhden pisteen.
 *
 *
 * Ohjelman kirjoittaja
 * Nimi: Inka Rannikko
 * Opiskelijanumero: 292034
 * Käyttäjätunnus: nfinra ( Git-repositorion hakemistonimi. )
 * E-Mail: inka.rannikko@tuni.fi
 * Palautteen kieli (fi/en): fi
 * */

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void move();
    void fruit_drop();
    void draw_fruit();
    void time_increaser();
    void find_matches();
    void on_EndGameButton_clicked();

private:
    Ui::MainWindow *ui;

    // Scene for the game grid
    QGraphicsScene* scene_;

    // Margins for the drawing area (the graphicsView object)
    // You can change the values as you wish
    const int TOP_MARGIN = 150;
    const int LEFT_MARGIN = 100;

    // Size of a square containing a fruit
    const int SQUARE_SIDE = 50; // give your own value here
    // Number of vertical cells (places for fruits)
    const int ROWS = 8; // give your own value here
    // Number of horizontal cells (places for fruits)
    const int COLUMNS = 11; // give your own value here

    // Constants describing scene coordinates
    const int BORDER_UP = 0;
    const int BORDER_DOWN = SQUARE_SIDE * ROWS;
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = SQUARE_SIDE * COLUMNS;

    std::vector<std::vector<int >> gameboard = {{0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0},
                                                {0,0,0,0,0,0,0,0,0,0,0}
                                                };
    std::vector<std::vector<QPushButton*>> fruit_pointers = {{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
                                                             {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr}};

    std::vector<int>coords;
    std::vector<int> fruits_to_be_deleted;
    QTimer* game_timer;
    QTimer* timer;
    QTimer* delayer;
    int score = 0;
    int game_time = 0;

    // Constants for different fruits and the number of them
    // Add/remove fruits as you wish, or you can remove the whole enum type
    // Actually only the value NUMBER_OF_FRUITS is needed in the
    // template code
    enum Fruit_kind {PLUM,
                     STRAWBERRY,
                     APPLE,
                     LEMON,
                     BLUEBERRY,
                     ORANGE,
                     NUMBER_OF_FRUITS};

    // For randomly selecting fruits for the grid
    std::default_random_engine randomEng_;
    std::uniform_int_distribution<int> distr_;

    // Writes the titles for the grid rows and columns
    void init_titles();
    //lisää hedelmiä vastaavat numerot gameboardiin ja tallettaa painonappien osoittimet fruit_pointeriin
    void add_fruits();
    //tarkistaa löytyykö vierekkäisiä hedelmiä kolme tai enemmän
    bool check_fruits();
    //vaihtaa hedelmien paikkoja
    void flip();
    //tarkistaa sijaitsevatko liikuteltavat hedelmät vierekkäin pelilaudalla
    bool is_move_valid();
    //pudottaa hedelmät tyhjille paikoille
    void pop_fruits(std::vector<std::vector <int>> matched_fruits);




};
#endif // MAINWINDOW_HH
