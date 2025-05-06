#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <QDir>
#include <opencv2/opencv.hpp>

class VideoRecorderApp : public QWidget {
    Q_OBJECT
public:
    VideoRecorderApp(QWidget* parent = nullptr) : QWidget(parent) {
        // Video label
        videoLabel = new QLabel("Initializing...");
        videoLabel->setAlignment(Qt::AlignCenter);
        videoLabel->setStyleSheet("background-color: black;");

        // Timer overlay
        overlayLabel = new QLabel("● Recording: 00:00:00", videoLabel);
        overlayLabel->setStyleSheet("color: red; font-size: 16px; font-weight: bold;"
                                    "background-color: rgba(0, 0, 0, 128); padding: 4px;");
        overlayLabel->move(10, 10);
        overlayLabel->hide();

        // Buttons
        startButton = new QPushButton("Start Recording");
        stopButton = new QPushButton("Stop Recording");
        stopButton->setEnabled(false);

        // Layouts
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(videoLabel);

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(startButton);
        buttonLayout->addWidget(stopButton);
        mainLayout->addLayout(buttonLayout);

        // Connect
        connect(startButton, &QPushButton::clicked, this, &VideoRecorderApp::startRecording);
        connect(stopButton, &QPushButton::clicked, this, &VideoRecorderApp::stopRecording);

        // Camera init
        cap.open(0);
        if (!cap.isOpened()) {
            videoLabel->setText("Camera not found.");
            return;
        }

        // Frame refresh
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &VideoRecorderApp::updateFrame);
        timer->start(30); // 30 ms for ~33 FPS

        // Recording time
        recordTimer = new QTimer(this);
        connect(recordTimer, &QTimer::timeout, this, &VideoRecorderApp::updateRecordingTime);
    }

    ~VideoRecorderApp() {
        cap.release();
        if (writer.isOpened()) writer.release();
    }

private slots:
    void updateFrame() {
        cap.read(frame);
        if (frame.empty()) return;

        if (recording && writer.isOpened()) {
            writer.write(frame);
        }

        cv::Mat rgbFrame;
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
        QImage img(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);
        videoLabel->setPixmap(QPixmap::fromImage(img).scaled(videoLabel->size(), Qt::KeepAspectRatio));
    }

    void startRecording() {
        QString outputPath = QDir::homePath() + "/Movies/";
        QString filename = outputPath + "recording_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".mp4";

        int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        double fps = cap.get(cv::CAP_PROP_FPS);
        if (fps == 0.0) fps = 30.0;

        writer.open(filename.toStdString(), cv::VideoWriter::fourcc('m','p','4','v'), fps, cv::Size(width, height));
        if (!writer.isOpened()) {
            videoLabel->setText("Failed to record.");
            return;
        }

        recording = true;
        elapsedSeconds = 0;
        showDot = true;
        overlayLabel->show();
        updateRecordingTime(); // force first update
        recordTimer->start(1000);

        startButton->setEnabled(false);
        stopButton->setEnabled(true);
    }

    void stopRecording() {
        recording = false;
        writer.release();
        recordTimer->stop();
        overlayLabel->hide();

        startButton->setEnabled(true);
        stopButton->setEnabled(false);
    }

    void updateRecordingTime() {
        elapsedSeconds++;
        showDot = !showDot;

        int h = elapsedSeconds / 3600;
        int m = (elapsedSeconds % 3600) / 60;
        int s = elapsedSeconds % 60;
        QString dot = showDot ? "●" : " ";
        overlayLabel->setText(QString("%1 Recording: %2:%3:%4")
            .arg(dot)
            .arg(h, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0')));
    }

private:
    QLabel* videoLabel;
    QLabel* overlayLabel;
    QPushButton* startButton;
    QPushButton* stopButton;
    QTimer* timer;
    QTimer* recordTimer;
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    cv::Mat frame;
    bool recording = false;
    int elapsedSeconds = 0;
    bool showDot = true;
};

#include "video_recorder.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    VideoRecorderApp window;
    window.setWindowTitle("Open Source Webcam Recorder");
    window.resize(800, 600);
    window.show();
    return app.exec();
}
