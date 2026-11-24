/*
 *  SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_num_parser.h"

#include <qnumeric.h>
#include <qmath.h>
#include <QVector>
#include <QRegularExpression>
#include <QStringList>
#include <QVariant>
#include <QLocale>

#include <iostream>

using namespace std;

const QVector<char> opLevel1 = {'+', '-'};
const QVector<char> opLevel2 = {'*', '/'};

const QStringList supportedFuncs = {"", "cos", "sin", "tan", "acos", "asin", "atan", "exp", "ln", "log10", "abs"};

const QRegularExpression funcExpr("^(-)?([a-zA-Z]*[0-9]*)?\\((.+)\\)$");
const QRegularExpression numberExpr("^(-)?([0-9]+\\.?[0-9]*(e[0-9]*)?)$");

const QRegularExpression funcExprInteger("^(-)?\\((.+)\\)$");
const QRegularExpression integerExpr("^(-)?([0-9]+)$");

double treatFuncs(QString const& expr, bool & noProblem);
double treatLevel1(QString const& expr, bool & noProblem);
double treatLevel2(QString const& expr, bool & noProblem);
double treatLevel3(QString const& expr, bool & noProblem);

double treatLevel1Int(QString const& expr, bool & noProblem);
double treatLevel2Int(QString const& expr, bool & noProblem);
double treatFuncsInt(QString const& expr, bool & noProblem);

namespace KisNumericParser {

double parseSimpleMathExpr(const QString &expr, bool *noProblem)
{

    bool ok = true;

    if (noProblem != nullptr) {
        return treatLevel1(expr, *noProblem);
    }

    return treatLevel1(expr, ok);

}

int parseIntegerMathExpr(QString const& expr, bool* noProblem)
{

    bool ok = true;

    if (noProblem != nullptr) {
        return qRound(treatLevel1Int(expr, *noProblem));
    }

    return qRound(treatLevel1Int(expr, ok));

}

}

inline QString extractSubExprLevel1(QString & expr, char & nextOp, bool & noProblem){

    QString ret;

    int subCount = 0;

    bool lastMetIsNumber = false;

    for(int i = 0; i < expr.size(); i++){

    if (expr.at(i) == '(') {
        subCount++;
    }

    if (expr.at(i) == ')') {
        subCount--;
    }

    if (subCount < 0) {
        noProblem = false;
        return ret;
    }

    if(i == expr.size()-1 && subCount == 0){
        ret = expr;
        expr.clear();
        break;
    }

    if( (expr.at(i) == '+' || expr.at(i) == '-') &&
            subCount == 0) {

        if (expr.at(i) == '-' &&
                i < expr.size()-1) {

            bool cond = !expr.at(i+1).isSpace();

            if (cond && !lastMetIsNumber) {
                continue;
            }

        }

        ret = expr.mid(0, i).trimmed();
        nextOp = expr.at(i).toLatin1();
        expr = expr.mid(i+1);
        break;

    }

    if (expr.at(i).isDigit()) {
        lastMetIsNumber = true;
    } else if (expr.at(i) != '.' &&
              !expr.at(i).isSpace()) {
        lastMetIsNumber = false;
    }

    }

    noProblem = true;
    return ret;
}

inline QString extractSubExprLevel2(QString & expr, char & nextOp, bool & noProblem){

    QString ret;

    int subCount = 0;

    for(int i = 0; i < expr.size(); i++){

    if (expr.at(i) == '(') {
        subCount++;
    }

    if (expr.at(i) == ')') {
        subCount--;
    }

    if (subCount < 0) {
        noProblem = false;
        return ret;
    }

    if(i == expr.size()-1 && subCount == 0){
        ret = expr;
        expr.clear();
        break;
    }

    if( (expr.at(i) == '*' || expr.at(i) == '/') &&
            subCount == 0) {

        ret = expr.mid(0, i).trimmed();
        nextOp = expr.at(i).toLatin1();
        expr = expr.mid(i+1);
        break;

    }

    }

    noProblem = true;
    return ret;
}

double treatLevel1(const QString &expr, bool & noProblem)
{

    noProblem = true;

    QString exprDestructible = expr;

    char nextOp = '+';
    double result = 0.0;

    while (!exprDestructible.isEmpty()) {

        double sign = (nextOp == '-') ? -1 : 1;
        QString part = extractSubExprLevel1(exprDestructible, nextOp, noProblem);

        if (!noProblem) {
        return 0.0;
        }

        if (sign > 0) {
        result += treatLevel2(part, noProblem);
        } else {
        result -= treatLevel2(part, noProblem);
        }

        if(!noProblem){
        return 0.0;
        }
    }

    return result;

}

double treatLevel2(QString const& expr, bool & noProblem)
{

    noProblem = true;

    QString exprDestructible = expr;

    char nextOp = '*';

    QString part = extractSubExprLevel2(exprDestructible, nextOp, noProblem);

    double result = treatLevel3(part, noProblem);

    while (!exprDestructible.isEmpty()) {

        if (!noProblem) {
        return 0.0;
        }

        bool needToMultiply = (nextOp == '*');
        part = extractSubExprLevel2(exprDestructible, nextOp, noProblem);

        if (!noProblem) {
        return 0.0;
        }

        if (needToMultiply) {
        result *= treatLevel3(part, noProblem);
        } else {
        result /= treatLevel3(part, noProblem);
        }
    }

    return result;
}

double treatLevel3(const QString &expr, bool & noProblem)
{

    noProblem = true;

    int indexPower = -1;
    int indexCount = 0;
    int subLevels = 0;

    for (int i = 0; i < expr.size(); i++) {
        if (expr.at(i) == '(') {
            subLevels++;
        } else if(expr.at(i) == ')') {
            subLevels--;
            if (subLevels < 0) {
                noProblem = false;
                return 0.0;
            }
        } else if (expr.at(i) == '^') {
            if (subLevels == 0) {
                indexPower = i;
                indexCount++;
            }
        }
    }

    if (indexCount > 1 || indexPower + 1 >= expr.size()) {
        noProblem = false;
        return 0.0;
    }

    if (indexPower > -1) {

        QStringList subExprs;
        subExprs << expr.mid(0,indexPower);
        subExprs << expr.mid(indexPower+1);

        bool noProb1 = true;
        bool noProb2 = true;

        double base = treatFuncs(subExprs[0], noProb1);
        double power = treatFuncs(subExprs[1], noProb2);

        return qPow(base, power);

    } else {
        return treatFuncs(expr, noProblem);
    }

    noProblem = false;
    return 0.0;

}

double treatFuncs(QString const& expr, bool & noProblem)
{

    noProblem = true;

    QRegularExpression funcExp = funcExpr;
    QRegularExpression numExp = numberExpr;
    QRegularExpressionMatch match;

    if (expr.trimmed().contains(funcExp, &match)) {

        int sign = match.captured(1).isEmpty() ? 1 : -1;
        QString func = match.captured(2).toLower();
        QString subExpr = match.captured(3);

        double val = treatLevel1(subExpr, noProblem);

        if (!noProblem) {
            return 0.0;
        }

        if (func.isEmpty()) {
            return sign*val;
        }

        if (!supportedFuncs.contains(func)) {
            noProblem = false;
            return 0.0;
        }

        if (func == "cos") {
            val = qCos(val/180*qAcos(-1));
        } else if (func == "sin") {
            val = qSin(val/180*qAcos(-1));
        } else if (func == "tan") {
            val = qTan(val/180*qAcos(-1));
        } else if(func == "acos") {
            val = qAcos(val)*180/qAcos(-1);
        } else if (func == "asin") {
            val = qAsin(val)*180/qAcos(-1);
        } else if (func == "atan") {
            val = qAtan(val)*180/qAcos(-1);
        } else if (func == "exp") {
            val = qExp(val);
        } else if (func == "ln") {
            val = qLn(val);
        } else if (func == "log10") {
            val = qLn(val)/qLn(10.0);
        } else if (func == "abs") {
            val = qAbs(val);
        }

        return sign*val;
    } else if(expr.trimmed().contains(numExp)) {
        return expr.toDouble(&noProblem);
    }

    double val = QLocale().toDouble(expr, &noProblem);

    if(noProblem) {
        return val;
    }

    noProblem = false;
    return 0.0;

}

double treatLevel1Int(QString const& expr, bool & noProblem)
{

    noProblem = true;

    QString exprDestructible = expr;

    char nextOp = '+';
    double result = 0.0;

    while (!exprDestructible.isEmpty()) {

    double sign = (nextOp == '-') ? -1 : 1;
    QString part = extractSubExprLevel1(exprDestructible, nextOp, noProblem);

    if( !noProblem) {
        return 0.0;
    }

    if (sign > 0) {
        result += treatLevel2Int(part, noProblem);
    } else {
        result -= treatLevel2Int(part, noProblem);
    }

    if(!noProblem){
        return 0.0;
    }
    }

    return result;

}

double treatLevel2Int(const QString &expr, bool &noProblem)
{

    noProblem = true;

    QString exprDestructible = expr;

    char nextOp = '*';

    QString part = extractSubExprLevel2(exprDestructible, nextOp, noProblem);

    double result = treatFuncsInt(part, noProblem);

    while (!exprDestructible.isEmpty()) {

    if (!noProblem) {
        return 0.0;
    }

    bool needToMultiply = (nextOp == '*');
    part = extractSubExprLevel2(exprDestructible, nextOp, noProblem);

    if (!noProblem) {
        return 0.0;
    }

    if (needToMultiply) {
        result *= treatFuncsInt(part, noProblem);
    } else {

        double val = treatFuncsInt(part, noProblem);

        if(std::isinf(result/val) || qIsNaN(result/val)){
        noProblem = false;
        return 0.0;
        }

        result /= val;
    }
    }

    return result;

}

double treatFuncsInt(QString const& expr, bool & noProblem)
{

    noProblem = true;

    QRegularExpression funcExpInteger = funcExprInteger;
    QRegularExpression numberExp = numberExpr;
    QRegularExpressionMatch match;

    if (expr.trimmed().contains(funcExpInteger, &match)) {

        int sign = match.captured(1).isEmpty() ? 1 : -1;
        QString subExpr = match.captured(2);

        double val = treatLevel1Int(subExpr, noProblem);

        if (!noProblem) {
            return 0;
        }

        return sign*val;

    } else if(expr.trimmed().contains(numberExp)) {
        double value = QVariant(expr).toDouble(&noProblem);
        return value;
    }

    noProblem = false;
    return 0;

}
