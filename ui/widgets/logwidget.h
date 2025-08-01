#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

class QTextEdit;

class LogWidget : public QWidget
{
    Q_OBJECT
public:
    // LOG TYPE
    static constexpr int INFO = 0;
    static constexpr int SUCCESS = 1;
    static constexpr int WARNING = 2;
    static constexpr int DANGER = 3;

    // Must be called once before any usage, e.g. in main window/widget
    static void init(QWidget* parent = nullptr);
    static LogWidget* instance();

    static bool addLog(const QString& text, int type = LogWidget::INFO);
    static void clear();

signals:

private:
    static LogWidget* d_instance;
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();
    void createInterface();
    QTextEdit *d_logtext;

    /**
     * @brief addLog ajouter un text au log
     * @param text: texte à ajouter
     * @param type: le type de log. les types sont:
     *  LogWidget::INFO
     *  LogWidget::SUCCESS
     *  LogWidget::WARNING
     *  LogWidget::DANGER
     * @return true si le text a été ajouté avec succés, false sinon
     */
    bool log(const QString& text, int type = LogWidget::INFO);
};

#endif // LOGWIDGET_H
