#include "mainwindow.h"
#include "ui_mainwindow.h"
#define MAXNETWORKSIZE 6
#define OFFSET 120
#define RECTX 100
#define RECTY 40
#define HYPLEN 250
#define PACKETTIME 2000000
#define MIDTIME 1000000
QList<ConnectedNetworks> networkList;
QList<PacketInfo> currentPackets;
QList<ConnectedNetworks> clientList;
int fileRows = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_mainClient_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    mw = this;
    setHeaders();

}

void MainWindow::on_mainServer_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    mw = this;
}

void MainWindow::on_mainForwarder_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    mw = this;
    setGraphics();

    pthread_t ok;
   pthread_create(&ok,NULL, theThread, (void * )0);

}

void MainWindow::on_serverStart_clicked()
{
    int port, udpPort;
    udpPort = ui->udpPortEdit->text().toInt();
    port = ui->lineEdit->text().toInt();
    startServer(port, udpPort);
}

void MainWindow::on_startForward_clicked()
{
    startForwarder();
}
void MainWindow::setTextView(QString s){
    ui->webViewer->clear();
    ui->webViewer->setText(s);
}
void MainWindow::setImageView(QString s) {
    QUrl Uri (QString ( "./outFile" ));
    QImage image = QImage("./outFile");

    ui->webViewer->clear();
    QTextDocument * textDocument = ui->webViewer->document();
    textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant ( image ) );
    QTextCursor cursor = ui->webViewer->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth( image.width() );
    imageFormat.setHeight( image.height() );
    imageFormat.setName( Uri.toString() );
    cursor.insertImage(imageFormat);
}

void MainWindow::on_startClient_clicked()
{
    int port = ui->clientForwardPort->text().toInt();
    QString ip = ui->clientForwardIP->text();
    connectClient(port, ip.toStdString().c_str());
    ui->startClient->setEnabled(false);

}
void MainWindow::removeNetwork(QString ip, int port){
    for(int i = 0; i < clientList.length(); i++){
        if(clientList[i].IP == ip && clientList[i].port == port){
            clientList.removeAt(i);
            drawGraph();
            return;
        }
    }
    for(int i = 0; i < networkList.length(); i++){
        if(networkList[i].IP == ip && networkList[i].port == port){
            networkList.removeAt(i);
            drawGraph();

            return;
        }
    }
}

void MainWindow::setHeaders(){
    ui->fileWidget->setRowCount(0);
    ui->fileWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->fileWidget->verticalHeader()->hide();
    ui->fileWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->fileWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->fileWidget->setColumnCount(2);
    int size = ui->fileWidget->width();

    ui->fileWidget->setColumnWidth(0, .7 * size);
    ui->fileWidget->setColumnWidth(1, .3 * size);

    ui->fileWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("File"));
    ui->fileWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
}

void MainWindow::addFileRow(QString name, QString type){

    ui->fileWidget->insertRow(fileRows);
    ui->fileWidget->setItem(fileRows, 0, new QTableWidgetItem(name));
    ui->fileWidget->setItem(fileRows, 1, new QTableWidgetItem(type));
    fileRows++;
}

void MainWindow::setGraphics(){
    brushes = QBrush(Qt::red);
    pens[0] = QPen(Qt::black);
    pens[0].setWidth(4);
    pens[1] = QPen(Qt::black);
    pens[1].setWidth(2);
    pens[2] = QPen(Qt::green);
    pens[2].setWidth(5);
    pens2 = QPen(Qt::blue);

    scene = new QGraphicsScene(0,0,700,500);

    ui->networkView->setScene(scene);
    //int test[] = {20,40,60,80,199,80,60,40,20,10, 30, 30, 30, 40, 50, 99,320 };
  //  drawGraph();


}
void MainWindow::drawGraph(){

    int middle;
    int theMid = networkList.length() /2;
    bool even = false;
    if((networkList.length() %2) == 0){
        middle = ui->networkView->width()/2 - OFFSET/2;
        theMid--;
        even = true;
    }
    else{
        middle = ui->networkView->width()/2;

    }
    int pyth;
    int REALMID = ui->networkView->height();
    int hPyth;
    int dif;
    int m1,m2;
    for(int i = 0; i < networkList.length(); i++){
        networkList[i].pos = i;
        networkList[i].x = middle + (i - theMid)*OFFSET;
        hPyth =  ui->networkView->width()/2 - networkList[i].x;
        pyth =  HYPLEN * HYPLEN - hPyth * hPyth;
        if(pyth < 0)
            pyth *= -1;
        //networkList[i].y = sqrt(pyth) + REALMID/2;

        dif = theMid -i;
        if(even){
           if(dif < 0){
               dif = theMid -i + 1;
           }
        }
        if(dif < 0)
            dif*=-1;
        networkList[i].y = REALMID - dif *50;

    }
    theMid = clientList.length() /2;
    even = false;
    if((clientList.length() %2) == 0){
        theMid--;
        middle = ui->networkView->width()/2 - OFFSET/2;

        even = true;
    }

    for(int i = 0; i < clientList.length(); i++){
        clientList[i].pos = i;
        clientList[i].x = middle + (i - theMid)*OFFSET;
        hPyth =  ui->networkView->width()/2 - networkList[i].x;
        pyth =  HYPLEN * HYPLEN - hPyth * hPyth;
        if(pyth < 0)
            pyth *= -1;
        //networkList[i].y = sqrt(pyth) + REALMID/2;

        dif = theMid -i;
        if(even){
           if(dif < 0){
               dif = theMid -i + 1;
           }
        }
        if(dif < 0)
            dif*=-1;
        clientList[i].y = 150 + dif *50;

    }

    drawBackground();


}
void MainWindow::addClient(QString ip, int port){
    ConnectedNetworks c;
    c.IP = ip;
    c.port = port;
    clientList.push_back(c);
    drawGraph();

}
void MainWindow::addNetwork(QString ip, int port){
    ConnectedNetworks c;
    c.IP = ip;
    c.port = port;
    networkList.push_back(c);
    drawGraph();
}
void MainWindow::drawBackground(){
    scene->clear();

    int REALMID = ui->networkView->height() + 100;

    for(int i = 0; i < networkList.length(); i++){
        QLineF line(ui->networkView->width()/2, REALMID/2,  networkList[i].x,REALMID - networkList[i].y );
        scene->addLine(line,pens[0]);

        scene->addRect(networkList[i].x - RECTX/2,REALMID - networkList[i].y -RECTY/2, RECTX,RECTY, pens[0], brushes);
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPos(networkList[i].x - RECTX/2, REALMID - networkList[i].y -RECTY/2);
        io->setPlainText(networkList[i].IP);


        scene->addItem(io);

    }
    for(int i = 0; i < clientList.length(); i++){
        QLineF line(ui->networkView->width()/2, REALMID/2,  clientList[i].x,REALMID - clientList[i].y );
        scene->addLine(line,pens[0]);

        scene->addRect(clientList[i].x - RECTX/2,REALMID - clientList[i].y -RECTY/2, RECTX,RECTY, pens[0], QBrush(Qt::blue));
        QGraphicsTextItem * io = new QGraphicsTextItem;
        io->setPos(clientList[i].x - RECTX/2, REALMID - clientList[i].y -RECTY/2);
        io->setPlainText(clientList[i].IP);


        scene->addItem(io);

    }
    qApp->processEvents();

}
void * theThread(void * args){
    while(1){
        usleep(30000);
        drawLoop();
    }
}

void MainWindow::addPacket(QString src, QString dst, int srcPort, int dstPort, bool cache){
    PacketInfo p;
    for(int i = 0; i < networkList.length(); i++){
        if(networkList[i].IP == dst && networkList[i].port == dstPort){
            p.endIndex = i;
            p.request = true;
            p.startTime = clockC();
            p.midTime = p.startTime + MIDTIME;
            p.endTime = p.startTime + PACKETTIME;
            p.startIndex = -1;

            p.midX = ui->networkView->width()/2;
            p.midY = (ui->networkView->height()+100)/2;
            p.endY = networkList[i].y;
            p.endX = networkList[i].x;
            for(int z = 0; z < clientList.length(); z++){
                if(clientList[z].IP == src && clientList[z].port == srcPort){
                    p.startY = clientList[z].y;
                    p.startX = clientList[z].x;
                }
            }
            p.startTime = clockC();

            p.midTime = p.startTime + MIDTIME;
            p.endTime = p.startTime + PACKETTIME;
            if(cache){
                p.endTime = p.midTime;
            }
            p.startIndex = i;
            currentPackets.push_back(p);
            return;
        }
    }
    for(int i = 0; i < networkList.length(); i++){
        if(networkList[i].IP == src && networkList[i].port == srcPort){
            p.endIndex = -1;
            p.startX = networkList[i].x;
            p.startY = networkList[i].y;

            p.midX = ui->networkView->width()/2;
            p.midY = (ui->networkView->height()+100)/2;
            for(int z = 0; z < clientList.length(); z++){
                if(clientList[z].IP == dst && clientList[z].port == dstPort){
                    p.endY = clientList[z].y;
                    p.endX = clientList[z].x;


                }
            }
            p.request = true;
            p.startTime = clockC();
            for(int z = 0; z < currentPackets.length();z++){
                if(currentPackets[z].endIndex == i)
                    p.startTime = currentPackets[z].endTime;

            }
            for(int z = currentPackets.length(); z > 0 ;z--){
                if(currentPackets[z-1].startX == p.startX && currentPackets[z-1].startY == p.startY){
                    p.startTime = currentPackets[z-1].startTime + 100000;
                    break;
                }

            }

          //  p.startTime = clockC();
            p.midTime = p.startTime + MIDTIME;

            p.endTime = p.startTime + PACKETTIME;
            if(cache){
                p.midTime = p.startTime;
                p.endTime = p.startTime + MIDTIME;
            }
            p.startIndex = i;
            currentPackets.push_back(p);
            return;
        }
    }

}
void drawLoop(){
    if(currentPackets.length() == 0)
       return;
    double perc;
    double curTime = clockC();
    int curX, curY;

    QMetaObject::invokeMethod(mw, "drawBackground");
    for(int i = 0; i < currentPackets.length(); i++){
        if(curTime < currentPackets[i].startTime)
            continue;
        if(curTime < currentPackets[i].midTime){
            perc = curTime - currentPackets[i].startTime;
            perc /= MIDTIME;
            curX = (currentPackets[i].midX - currentPackets[i].startX) * perc + currentPackets[i].startX;
            curY = (currentPackets[i].midY - currentPackets[i].startY) * perc + currentPackets[i].startY;
            QMetaObject::invokeMethod(mw, "addDotToNetwork", Q_ARG(int, curX), Q_ARG(int, curY));

        }else{
            perc = curTime - currentPackets[i].midTime;
            perc /= MIDTIME;
            curX = (currentPackets[i].endX - currentPackets[i].midX) * perc + currentPackets[i].midX;
            curY = (currentPackets[i].endY - currentPackets[i].midY) * perc + currentPackets[i].midY;
            QMetaObject::invokeMethod(mw, "addDotToNetwork", Q_ARG(int, curX), Q_ARG(int, curY));
            if(curTime > currentPackets[i].endTime){
                currentPackets.removeAt(i);
                QMetaObject::invokeMethod(mw, "drawBackground");

            }

        }

    }



}
void MainWindow::addDotToNetwork(int x, int y){
    scene->addEllipse(x, 600-y, 5,5,pens[2], brushes);

}

void MainWindow::on_secondGo_clicked()
{
    int row = ui->fileWidget->selectionModel()->currentIndex().row();
    if(row == -1)
        return;

    QString fileName = ui->fileWidget->item(row, 0)->text();

    QString dstIp, dstPort, fileType;
    dstIp = ui->clientForwardIP->text();
    dstPort = ui->clientDestPort->text();
    fileType = ui->fileWidget->item(row, 1)->text();

    requestFile(fileName, dstIp, fileType, dstPort.toInt(), true);
}

void MainWindow::on_pushButton_clicked()
{
    QString ip;
    int port;
    requestUdp("w1", ui->clientForwardIP->text(), ui->clientDestPort->text().toInt(), 7777);
}
