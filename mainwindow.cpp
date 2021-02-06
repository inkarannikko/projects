#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QDebug>
#include <QPixmap>
#include <vector>

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
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles
    scene_ = new QGraphicsScene(this);

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(LEFT_MARGIN, TOP_MARGIN,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a fruit is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    int seed = time(0); // You can change seed value for testing purposes
    randomEng_.seed(seed);
    distr_ = std::uniform_int_distribution<int>(0, NUMBER_OF_FRUITS - 1);
    distr_(randomEng_); // Wiping out the first random number (which is almost always 0)

    init_titles();
    while(!check_fruits()){
        add_fruits();
    }


    //kello pelatun ajan seuraamiselle
    game_timer = new QTimer(this);
    connect(game_timer, &QTimer::timeout, this, &MainWindow::time_increaser);
    game_timer->start(1000);

    //kello funktiokutsuja varten
    timer = new QTimer(this);
    delayer = new QTimer(this); //viivästää hedelmien putoamista
    connect(timer, &QTimer::timeout, this, &MainWindow::fruit_drop);
    connect(timer, &QTimer::timeout, this, &MainWindow::draw_fruit);
    connect(timer, &QTimer::timeout, this, &MainWindow::find_matches);
    timer->start(10);




    //taulu pisteille
    ui->ScoreLabel->setText("Score: 0");

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*funktio kerää talteen painettuejn nappien koordinaatit niiden vaihtoa varten*/
void MainWindow::move()
{

    QPushButton* pButton = qobject_cast<QPushButton*>(sender());
    if (pButton) // this is the type we expect
    {
        int x = (pButton->x()/50)-2;
        int y = pButton->y()/50-3;
        coords.push_back(x);
        coords.push_back(y);

        if(coords.size()== 4){
            flip();
            coords.clear();

        }
    }
}


void MainWindow::init_titles()
{
    // Displaying column titles, starting from A
    for(int i = 0, letter = 'A'; i < COLUMNS; ++i, ++letter)
    {
        int shift = 5;
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN + shift + i * SQUARE_SIDE,
                           TOP_MARGIN - SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }

    // Displaying row titles, starting from A
    for(int i = 0, letter = 'A'; i < ROWS; ++i, ++letter)
    {
        QString letter_string = "";
        letter_string.append(letter);
        QLabel* label = new QLabel(letter_string, this);
        label->setGeometry(LEFT_MARGIN - SQUARE_SIDE,
                           TOP_MARGIN + i * SQUARE_SIDE,
                           SQUARE_SIDE, SQUARE_SIDE);
    }
}

/*arpoo pelin alussa hedelmät ja täyttää laudan painonapeilla
 * osoittimet painonappeihin tallennetaan matriisiin
 * arvottuja hedelmiä vastaavat numerot tallennetaan matriisiin*/
void MainWindow::add_fruits()
{
    for(int j=0; j < ROWS; ++j){
        for(int i =0; i < COLUMNS; ++i){
            gameboard.at(j).at(i) = distr_(randomEng_);
            QPushButton *button = new QPushButton(this);
            fruit_pointers.at(j).at(i) = button;
            button->setGeometry(QRect((i+2)*SQUARE_SIDE, (j+3)*SQUARE_SIDE, SQUARE_SIDE,SQUARE_SIDE));
            connect(button, &QPushButton::clicked, this, &MainWindow::move);

        }
    }
}

/*tarkistaa löytyykö pelilaudalta kolme tai useampi vieräkkäinen hedelmä eli osuma
 * false jos löytyy ja true jos ei löydy*/
bool MainWindow::check_fruits()
{
    for(int j=0; j < ROWS; ++j){
        for(int i =0; i < COLUMNS; ++i){
            if(gameboard.at(j).at(i) == 10){
                continue;
            }
            if(i > 1 and j > 1 ){
                if(gameboard.at(j).at(i-2) == gameboard.at(j).at(i) and gameboard.at(j).at(i-1) == gameboard.at(j).at(i))
                    return false;
                if(gameboard.at(j-2).at(i) == gameboard.at(j).at(i) and gameboard.at(j-1).at(i) == gameboard.at(j).at(i))
                    return false;
            }
            else if( i > 1){
                if(gameboard.at(j).at(i-2) == gameboard.at(j).at(i) and gameboard.at(j).at(i-1) == gameboard.at(j).at(i))
                    return false;
            }else if( j > 1){
                if(gameboard.at(j-2).at(i) == gameboard.at(j).at(i) and gameboard.at(j-1).at(i) == gameboard.at(j).at(i))
                    return false;
            }

        }
    }
    return true;

}

/*vaihtaa kahden peräkkäisen hedelmän paikat jos niistä tulee match*/
void MainWindow::flip()
{
    timer->singleShot(5000, this, &MainWindow::fruit_drop);
    int first = gameboard.at(coords.at(1)).at(coords.at(0));
    int second = gameboard.at(coords.at(3)).at(coords.at(2));
    int first_copy = gameboard.at(coords.at(1)).at(coords.at(0));
    int second_copy = gameboard.at(coords.at(3)).at(coords.at(2));
    if(is_move_valid()){
        gameboard.at(coords.at(1)).at(coords.at(0)) = second;
        gameboard.at(coords.at(3)).at(coords.at(2)) = first;
        if(!check_fruits()){


        }


        else{
            gameboard.at(coords.at(1)).at(coords.at(0)) = first_copy;
           gameboard.at(coords.at(3)).at(coords.at(2)) = second_copy;

        }
    }
}

/*tarkistaa että painetut hedelmänapit ovat vieräkkäin*/
bool MainWindow::is_move_valid()
{
    if(abs(coords.at(0)-coords.at(2)) == 0 and abs(coords.at(1)-coords.at(3)) ==1){
        return true;
    }if(abs(coords.at(0)-coords.at(2)) ==1 or abs(coords.at(1)-coords.at(3)) ==0){
        return true;
    }
    return false;
}

/*pudottaa hedelmät alaspäin jos niiden alla on tyhjää*/
void MainWindow::fruit_drop()
{
    for(int j=ROWS-1; j > 0; --j){
        for(int i =0; i < COLUMNS; ++i){
            if(gameboard.at(j).at(i) == 10 && gameboard.at(j-1).at(i) != 10){
                gameboard.at(j).at(i) = gameboard.at(j-1).at(i);
                gameboard.at(j-1).at(i) = 10;
            }
        }
    }
}

/*tarkistaa löytyykö x tai y suunnassa peräkkäisiä hedelmiä*/
void MainWindow::find_matches()
{
    std::vector< std::vector<int> > matched_coords; //sisältää peräkkäiset hedelmät x- ja y-suunnissa
    std::vector< int > temp; //tähän kerätään samat peräkkäiset hedelmät x-tai y-suunnassa
    int fruit_type;



   //------------------------------------------------x
    for(int j=0; j < ROWS; ++j){
        fruit_type = gameboard.at(j).at(0);
        if(temp.size() >= 6){
            matched_coords.push_back(temp);
        }
        temp.clear();
        for(int i =0; i < COLUMNS; ++i){

            if (gameboard.at(j).at(i) != fruit_type)
            {
                if(temp.size() >= 6){
                    matched_coords.push_back(temp);
                }
                temp.clear();
                temp.push_back(i);
                temp.push_back(j);
                fruit_type = gameboard.at(j).at(i);
            }else{
                if(gameboard.at(j).at(i) != 10)
                {
                    temp.push_back(i);
                    temp.push_back(j);
                    if(j == ROWS-1 and i == COLUMNS-1 && temp.size() >= 6)
                    {
                        matched_coords.push_back(temp);
                    }
                }
            }
        }
    }
  //------------------------------------------------y
    temp.clear();
    for(int i=0; i < COLUMNS; ++i){
        fruit_type = gameboard.at(0).at(i);
        if(temp.size() >= 6){
            matched_coords.push_back(temp);
        }
        temp.clear();

        for(int j =0; j < ROWS; ++j){

            if (gameboard.at(j).at(i) != fruit_type)
            {
                if(temp.size() >= 6){
                    matched_coords.push_back(temp);
                }
                temp.clear();
                temp.push_back(i);
                temp.push_back(j);
                fruit_type = gameboard.at(j).at(i);

            }else{
                if(gameboard.at(j).at(i) != 10 && gameboard.at(j).at(i) == fruit_type)
                {
                    temp.push_back(i);
                    temp.push_back(j);
                    if(j == ROWS-1 and i == COLUMNS-1 && temp.size() >= 6)
                    {
                        matched_coords.push_back(temp);
                    }
                }
            }
        }
    }
    pop_fruits(matched_coords);



}

/*poistaa kolmen tai useamman hedelmän suorat*/
void MainWindow::pop_fruits(std::vector<std::vector<int> > matched_fruits)
{
   int x;
   int y;
   bool to_x = true; //kertoo onko kyseessä x- vai y-koordinaatti jos x niin tosi
   for(auto list: matched_fruits){
       for(auto coord : list){
           if(to_x)
           {
               x = coord;
               to_x = false; //seuraava alkio vektorissa on y-koordinaatti

           }else{
               y = coord;
               fruit_pointers.at(y).at(x)->setIcon(QIcon());
               gameboard.at(y).at(x) = 10; //muutetaan tämä kohta tyhjäksi pelilaudalla
               score++; //lisätään pisteet
               to_x = true; //seuraava alkio vektorissa on x-koordinaatti
           }

       }
   }
  ui->ScoreLabel->setText("Score: " + QString::number(score)); //päivitetään pisteet
}


/*piirtää hedelmät painonappeihin*/
void MainWindow::draw_fruit()
{
    // Vector of fruits
    const std::vector<std::string>
            fruits = {"cherries", "strawberry", "orange", "pear", "apple",
                      "bananas", "tomato", "grapes", "eggplant"};

    // Defining where the images can be found and what kind of images they are
    const std::string PREFIX(":/");
    const std::string SUFFIX(".png");



    // Setting the pixmap for a new label
    for(int j=0; j < ROWS; ++j){
        for(int i =0; i < COLUMNS; ++i){
             // Converting image (png) to a pixmap
            //int fruit_type = distr_(randomEng_);
            if( gameboard.at(j).at(i) == 10){
                QPushButton* button = fruit_pointers.at(j).at(i);
                button->setIcon(QIcon());
            }else{
                std::string filename = PREFIX + fruits.at(gameboard.at(j).at(i)) + SUFFIX;
                QPixmap image(QString::fromStdString(filename));

                // Scaling the pixmap
                image = image.scaled(SQUARE_SIDE, SQUARE_SIDE);
                QIcon icon(image);

                //gameboard.at(j).at(i) = fruit_type;
                QPushButton* button = fruit_pointers.at(j).at(i);
                button->setIcon(icon);
                button->setIconSize(image.rect().size());
            }


    }
    }
}

/*kasvattaa peliaikaa ja ilmoittaa koko ajan siihen asti kulunutta aikaa*/
void MainWindow::time_increaser()
{
    game_time++;
    ui->TimeLabel->setText(QString::number(game_time) + "s.");


}

/*päättää pelin ja kertoo pisteet ja käytetyn ajan*/
void MainWindow::on_EndGameButton_clicked()
{
  game_timer->stop();
  QMessageBox::information(this, tr("Game over"), "You popped " + QString::number(score) + " fruits and your time was " + QString::number(game_time));
  delete ui;


}
