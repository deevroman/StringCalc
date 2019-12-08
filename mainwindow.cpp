#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QApplication>
#include <QObject>
#include <QFontDatabase>
#include <QFont>
#include <QString>
#include "vector.h"
#include <cmath>
#include <stack>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Angle = ui->Grad_box->isChecked();
    ui->ValuesForm->hide();
    connect(ui->Input, SIGNAL(textChanged(QString)), this, SLOT(calculate()));
    connect(ui->POLIZ_box, SIGNAL(clicked(bool)), this, SLOT(set_visible_poliz(bool)));
    connect(ui->Grad_box, SIGNAL(clicked()), this, SLOT(calculate()));
    connect(ui->Radian_box, SIGNAL(clicked()), this, SLOT(calculate()));
}
void MainWindow::set_visible_poliz(bool checked){
    ui->POLIZ->setVisible(checked);
}

QString MainWindow::clear_space(QString input){
    QString res;
    res.reserve(input.size());
    QString ops = "+-*/^()[],";
    input += ")";
    for (int i = 0; i + 1 < input.size(); i++) {
        if (input[i] == ' ' && input[i+1] == ' ')
            continue;
        if (input[i] == ' ') {
            bool br = false;
            for (auto now : ops) {
                if (now == input[i+1]) {
                    br = true;
                    break;
                }
            }
            if (br)
                continue;
            else
                res.push_back(input[i]);
        } else {
            res.push_back(input[i]);
            if (input[i+1] == ' ') {
                for (auto now : ops) {
                    if (now == input[i]) {
                        while(i + 1 < input.size() && input[i+1] == ' '){
                            i++;
                        }
                        break;
                    }
                }
            }
        }
    }
    return res;
}

void MainWindow::calculate(){
    Angle = ui->Grad_box->isChecked();
    try {
        QString input = clear_space(ui->Input->text());
        if(input.size() == 0){
            hideTable();
            throw -1;
        }
        my_stl::vector<Operand> POLIZ;
        try {
            parser(input, POLIZ);
            my_stl::vector<pair<QString, double>> tmp;
            try {
                QString poliz;
                get_str_poliz(POLIZ, poliz);
                ui->POLIZ->setText(poliz);
                ui->POLIZ->setToolTip(poliz);
                ui->Answer->setText(MainWindow::calc_poliz(POLIZ, tmp));
                hideTable();
            } catch (QString err) {
                ui->Answer->setText(err);
                hideTable();
            }
            ui->Answer->setStyleSheet("QLabel {\n color : black;\n}");
            ui->Input->setStyleSheet("QLineEdit{\n	color : black\n}");
        } catch (int x) {
            if(x == -1){
                throw -1;
            } else if(x == -2){
                my_stl::vector<pair<QString, double>> vals;
                calc_poliz(POLIZ, vals, true);
                for(auto now : POLIZ){
                    if(now.type == 3){
                        bool ok = true;
                        for(auto x : vals){
                            if (x.first == now.value) {
                                ok = false;
                                break;
                            }
                        }
                        if(ok)
                            vals.push_back({now.value, 0});
                    }
                }
                bool ok = ui->InputTable->rowCount() == (int)vals.size();
                for(int i = 0; i < ui->InputTable->rowCount() && ok; i++){
                    if (ui->InputTable->item(i, 0)->text() != vals[i].first) {
                        ok = false;
                    }
                }
                ui->Input->setStyleSheet("QLineEdit{\n	color : black\n}");
                QString poliz;
                get_str_poliz(POLIZ, poliz);
                ui->POLIZ->setText(poliz);
                ui->POLIZ->setToolTip(poliz);
                if(!ok || vals.size() == 0){
                    disconnect(ui->InputTable, SIGNAL(cellChanged(int, int)), this, SLOT(calculate()));
                    ui->InputTable->clear();
                    ui->InputTable->setEnabled(true);
                    ui->InputTable->clearContents();
                    ui->InputTable->setColumnCount(2);
                    ui->InputTable->setHorizontalHeaderLabels(QStringList() << trUtf8("Переменная")
                                                              << trUtf8("Значение"));
                    ui->InputTable->setRowCount(0);
                    ui->InputTable->setShowGrid(true);
                    ui->InputTable->setSelectionMode(QAbstractItemView::SingleSelection);
                    ui->InputTable->horizontalHeader()->setStretchLastSection(true);
                    for(int i = 0; i < vals.size(); i++){
                        ui->InputTable->insertRow(i);
                        ui->InputTable->setItem(i, 0, new QTableWidgetItem(vals[i].first));
                        ui->InputTable->setItem(i, 1, new QTableWidgetItem(""));
                        ui->InputTable->item(i, 0)->setFlags(Qt::ItemIsEnabled);
                        ui->InputTable->item(i, 1)->setBackgroundColor(Qt::white);
                    }
                    connect(ui->InputTable, SIGNAL(cellChanged(int, int)), this, SLOT(calculate()));
                    ui->Answer->setText(QString(""));
                } else {
                    bool any = true;
                    for(int i = 0; i < ui->InputTable->rowCount(); i++){
                        QString text = ui->InputTable->item(i, 1)->text();
                        if(text == ""){
                            any = false;
                            ui->InputTable->item(i, 1)->setBackgroundColor(Qt::white);
                            break;
                        }
                        try{
                            int pos = 0;
                            if(text[0] == '+' || text[0] == '-'){
                                if(text.size() > 1 && text[1].isDigit()){
                                    pos = 1;
                                } else {
                                    throw -1;
                                }
                            }
                            vals[i].second = readNum(text, pos);
                            if(text[0] == '-'){
                                vals[i].second *= -1;
                            }
                            if(pos != text.size()){
                                throw -1;
                            }
                        } catch (int x){
                            any = false;
                            ui->InputTable->item(i, 1)->setBackgroundColor(Qt::red);
                        }
                        if(!any){
                            break;
                        }
                        ui->InputTable->item(i, 1)->setBackgroundColor(Qt::white);
                    }
                    if(any){
                        ui->Answer->setStyleSheet("QLabel {\n color : black;\n}");
                        try {
                            QString poliz;
                            get_str_poliz(POLIZ, poliz);
                            ui->POLIZ->setText(poliz);
                            ui->POLIZ->setToolTip(poliz);
                            ui->Answer->setText(MainWindow::calc_poliz(POLIZ, vals));
                        } catch (QString err) {
                            ui->Answer->setText(err);
                        }
                    } else {
//                        ui->Answer->setStyleSheet("QLabel {\n color : silver;\n}");
                        ui->Answer->setText(QString(""));
//                        ui->POLIZ->setText(QString(""));
                    }
                }
                ui->InputTable->setEnabled(true);
                ui->ValuesForm->show();
            }
        }
    } catch (int x) {
        ui->InputTable->setEnabled(false);
//        ui->Answer->setStyleSheet("QLabel {\n color : silver;\n}");
        ui->Answer->setText(QString(""));
        ui->Input->setStyleSheet("QLineEdit{\n	color : red\n}");
        ui->POLIZ->setText(QString(""));
        ui->POLIZ->setToolTip(QString(""));
    }
}

void MainWindow::hideTable(){
    ui->ValuesForm->hide();
    ui->InputTable->setRowCount(0);
}

QString MainWindow::calc_poliz(my_stl::vector<Operand> &POLIZ, my_stl::vector<pair<QString, double>> &values, bool not_calc){
    std::stack<double> st;
    for(auto now : POLIZ){
        if(now.type == 0){
            st.push(now.value.toDouble());
        } else if(now.type == 1 || now.type == 2){
            if(now.cnt_args <= st.size()){
                my_stl::vector<double> args;
                for(int i = 0; i < now.cnt_args; i++){
                    args.push_back(st.top());
                    st.pop();
                }
                args.reverse();
                if(not_calc){
                    st.push(0.0);
                }
                else{
                    st.push(now.calc(args, Angle));
                }
            } else{
                throw -1;
            }
        }else if(now.type == 3){
            if(not_calc){
                st.push(now.value.toDouble());
            } else {
                for(auto p : values){
                    if(p.first == now.value){
                        st.push(p.second);
                    }
                }
            }
        } else if(now.type == 4){
            my_stl::vector<double> args; // (:
            if(not_calc){
                st.push(0.0);
            }
            else{
                st.push(now.calc(args, Angle));
            }
        }

    }
    if(st.size() != 1){
        throw -1;
    } else{
        return QString::number(roundDouble(st.top()));
    }
}

void MainWindow::get_str_poliz(my_stl::vector<Operand> &POLIZ, QString &poliz){
    for(auto &now : POLIZ){
        if((now.value == "+" || now.value == "-")
                && now.cnt_args == 1){
            poliz += now.value + QString("u ");
        } else{
            poliz += now.value + QString(" ");
        }
    }
}

double MainWindow::readNum(QString &s, int &pos){
    double ans = 0.0;
    int cnt_before_point = 1;
    int exp = 0;
    int minus = 1;
    int readMode = 0;
    const QString ops = "+-*/^)],";
    // 0 ожидание целой части
    // 1 считывание дробной части
    // 2 считывание мантиссы
    while(pos < s.size()){
        if(readMode == 0){
            if(s[pos] == '.'){
                if(pos + 1 < s.size() && s[pos+1].isDigit()){
                    readMode = 1;
                } else{
                    throw -1;
                }
            } else if (s[pos].isDigit()) {
                ans = 10*ans + s[pos].toLatin1() - '0';
            } else if(s[pos] == 'e'){
                if(pos + 1 >= s.size()){
                    throw -1;
                }
                if(s[pos+1] == '+' || s[pos+1] == '-'){
                    if(pos + 2 >= s.size()){
                        throw -1;
                    }
                    if(s[pos+2].isDigit()){
                        if(s[pos+1] == '-'){
                            minus = -1;
                        }
                        pos++;
                        readMode = 2;
                    } else{
                        throw -1;
                    }
                } else if(!s[pos+1].isDigit()){
                    throw -1;
                } else{
                    readMode = 2;
                }
            } else{
                bool ok = false;
                for(auto now : ops){
                    if (s[pos] == now) {
                        ok = true;
                        break;
                    }
                }
                if(!ok){
                    throw -1;
                } else {
                    break;
                }
            }
        } else if(readMode == 1){
            if(s[pos].isDigit()){
                ans += (s[pos].toLatin1() - '0') / pow(10, cnt_before_point);
                cnt_before_point++;
            } else{
                if(s[pos] == 'e'){
                    if(pos + 1 >= s.size()){
                        throw -1;
                    }
                    if(s[pos+1] == '+' || s[pos+1] == '-'){
                        if(pos + 2 >= s.size()){
                            throw -1;
                        }
                        if(s[pos+2].isDigit()){
                            if(s[pos+1] == '-'){
                                minus = -1;
                            }
                            pos++;
                            readMode = 2;
                        } else{
                            throw -1;
                        }
                    } else if(!s[pos+1].isDigit()){
                        throw -1;
                    } else{
                        readMode = 2;
                    }
                } else{
                    bool ok = false;
                    for(auto now : ops){
                        if (s[pos] == now) {
                            ok = true;
                            break;
                        }
                    }
                    if(!ok){
                        throw -1;
                    } else {
                        break;
                    }
                }
            }
        } else if(readMode == 2){
            if (s[pos].isDigit()) {
                exp = 10*exp + s[pos].toLatin1() - '0';
            } else{
                if(s[pos] == '.'){
                    throw -1;
                } else {
                    break;
                }
            }
        }
        pos++;
    }
    #ifndef STD_MATH
        double x = 1;
        for(int i = 0; i < exp; i++){
            x *= 10;
        }
        if(minus == -1){
            x = 1 / x;
        }
        return ans * x;
    #else
        return ans * pow(10, minus*exp);
    #endif
}

QString MainWindow::readStr(QString &s, int &pos){
    QString ans;
    while(pos < s.size()
          && ('a' <= s[pos] && s[pos] <= 'z' || 'A' <= s[pos] && s[pos] <= 'Z'
              || '0' <= s[pos] && s[pos] <= '9' || s[pos] == '_')){
        ans += s[pos];
        pos++;
    }
    return ans;
}

void MainWindow::parser(QString input, my_stl::vector<Operand> &ans){
    std::stack<Operand> st;
    bool inValue = false;
    input = "(" + input + ")";
    for(int i = 0; i < input.size(); i++){
        if(input[i] == '(' || input[i] == '['){
            st.push(Operand(-1, QString(input[i]), 0, 0));
        } else if(input[i] == ')' || input[i] == ']'){
            while(!st.empty()){
                if (st.top().type != 2 && st.top().value != QString('(') && st.top().value != QString('[') ) {
                    ans.push_back(st.top());
                    st.pop();
                } else{
                    if ((input[i] == ')' && st.top().value == QString('['))
                            || (input[i] == ']' && st.top().value == QString('('))){
                        throw -1;
                    }
                    break;
                }
            }
            if(st.empty()){
                throw -1;
            }
            Operand tmp = st.top();
            st.pop();
            if (tmp.type == 2) {
                ans.push_back(tmp);
            } else if(QString(input[i]) == ")" && tmp.value != "(" || QString(input[i]) == "]" && tmp.value != "["){
                throw -1;
            }
        } else if(input[i] == '+'){
            bool unar = false;
            for(auto now : CharBeforeUnar){
                if(input[i-1] == now){
                    unar = true;
                    break;
                }
            }
            if(unar){
                st.push(Operand(1, "+", 1, 3));
            } else{
                while(!st.empty()){
                    if(st.top().priority >= P_PLUS){
                        ans.push_back(st.top());
                        st.pop();
                    } else{
                        break;
                    }
                }
                st.push(Operand(1, "+", 2, 1));
            }
        } else if(input[i] == '-'){
            bool unar = false;
            for(auto now : CharBeforeUnar){
                if(input[i-1] == now){
                    unar = true;
                    break;
                }
            }
            if(unar){
                st.push(Operand(1, "-", 1, 3));
            } else{
                while(!st.empty()){
                    if(st.top().priority >= P_SUB){
                        ans.push_back(st.top());
                        st.pop();
                    } else{
                        break;
                    }
                }
                st.push(Operand(1, "-", 2, 1));
            }
        } else if(input[i] == '*'){
            while(!st.empty()){
                if(st.top().priority >= P_MUL){
                    ans.push_back(st.top());
                    st.pop();
                } else{
                    break;
                }
            }
            st.push(Operand(1, "*", 2, 2));
        } else if(input[i] == '/'){
            while(!st.empty()){
                if(st.top().priority >= P_DIV){
                    ans.push_back(st.top());
                    st.pop();
                } else{
                    break;
                }
            }
            st.push(Operand(1, "/", 2, 2));
        }
        else if(input[i] == '^'){
            while(!st.empty()){
                if(st.top().priority > P_POW){
                    ans.push_back(st.top());
                    st.pop();
                } else{
                    break;
                }
            }
            st.push(Operand(1, "^", 2, 5));
        }
        else if(input[i].isDigit() || input[i] == '.'){
            ans.push_back(Operand(0, QString::number((double)readNum(input, i)), 0, 4));
            i--;
        } else if('a' <= input[i] && input[i] <= 'z'
                  || 'A' <= input[i] && input[i] <= 'Z'
                  || input[i] == '_'){
             QString str = readStr(input, i);
             if(str == "e" || str == "pi"){
                 ans.push_back(Operand(4, str, 0, 0));
                 i--;
             } else if(str == "cos" || str == "sin" || str == "tg" || str == "ctg"){
                 st.push(Operand(2, str, 1, 0));
                 if(i >= input.size() || input[i] != '('){
                     throw -1;
                 }
             } else if(str == "log"){
                 st.push(Operand(2, str, 2, 0));
                 st.push(Operand(-1, "(", 0, 0));
                 if(i >= input.size() || input[i] != '('){
                     throw -1;
                 }
             } else {
                st.push(Operand(3, str, 0, 5));
                i--;
                inValue = true;
             }
        } else if (input[i] == ',') {
            while(!st.empty()){
                if (st.top().value != "(") {
                    ans.push_back(st.top());
                    st.pop();
                } else{
                    break;
                }
            }
            if(st.empty()){
                throw -1;
            }
            if(st.top().value != "("){
                throw -1;
            }
            st.pop();
        }
        else{
            throw -1;
        }
    }
    if(!st.empty())
        throw -1;
    if(inValue)
        throw -2;
}

MainWindow::~MainWindow()
{
    delete ui;
}
