#ifndef CONNECTEDCOMPONENTALGORITHM_H
#define CONNECTEDCOMPONENTALGORITHM_H

#include <QVector>

namespace CORE
{

class ConnectedComponentAlgorithm
{
public:
    ConnectedComponentAlgorithm(QVector<QVector<int>> matrix_in);

    void getOut(QVector<QVector<int>>& matrix, QVector<int> &labels);

private:
    int hkFind(int x);
    int hkUnion(int x, int y);
    int hkMakeSet();
    void hkInitialize(int max_labels);
    void hoshenKopelman(QVector<QVector<int>> &matrix_in);

private:
    bool m_bComputed;

    QVector<QVector<int>> m_vMatrixIn;
    QVector<QVector<int>> m_vMatrixOut;
    QVector<int> m_vLabels;
};

}

#endif // CONNECTEDCOMPONENTALGORITHM_H
