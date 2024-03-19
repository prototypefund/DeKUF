#include <QMainWindow>

namespace Ui {
class MainWindow;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

private:
    Ui::MainWindow* ui;
};
