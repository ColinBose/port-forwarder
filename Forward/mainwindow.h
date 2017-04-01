#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextDocument>
#include <QImage>
#include <QTextCursor>
#include <QTextImageFormat>
#include <QFileDialog>
#include <QImageReader>
#include <QStyle>
#include <QDesktopWidget>
#include "server.h"
#include "middle.h"
#include "client.h"
#include "globals.h"
#include <QGraphicsScene>
#include <QPen>
#include <QGraphicsTextItem>
#include "hateclock.h"
struct ConnectedNetworks{
    int pos;
    QString IP;
    int port;
    int x,y = 0;
};
struct PacketInfo{
    bool request;
    int endIndex;
    int startIndex;
    long startTime;
    long endTime;
    long midTime;
    int startX, startY, midX, midY, endX, endY;

};



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setGraphics();
    void drawGraph();
    void setHeaders();
    void enableClient(bool shouldEnable);
    void enableServer(bool shouldEnable);
    void enableForwarder(bool shouldEnable);


private slots:
    Q_SLOT void drawBackground();
    void on_mainClient_clicked();
    Q_SLOT void setTextView(QString s);
    Q_SLOT void setImageView(QString s);
    Q_SLOT void addNetwork(QString ip, int port);
    Q_SLOT void addClient(QString ip, int port);
    Q_SLOT void addPacket(QString src, QString dst, int srcPort, int dstPort, bool cache);
    Q_SLOT void addDotToNetwork(int x, int y);
    Q_SLOT void removeNetwork(QString ip, int port);
    void on_mainServer_clicked();
    Q_SLOT void addFileRow(QString name, QString type);
    void on_mainForwarder_clicked();

    void on_serverStart_clicked();

    void on_startForward_clicked();
    void on_startClient_clicked();

    void on_secondGo_clicked();

    void on_pushButton_clicked();
private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene, *scene2;
    QBrush brushes;
    QPen pens[3];
    QPen pens2;
    QGraphicsScene* scenes;
};void * theThread(void * args);
void drawLoop();


#endif // MAINWINDOW_H
