#include "connectedcomponentalgorithm.h"

using namespace CORE;

int ConnectedComponentAlgorithm::hkFind(int x)
{
    int y = x;

    while (m_vLabels[y] != y)
        y = m_vLabels[y];

    while (m_vLabels[x] != x)
    {
        int z = m_vLabels[x];
        m_vLabels[x] = y;
        x = z;
    }

    return y;
}

int ConnectedComponentAlgorithm::hkUnion(int x, int y)
{
    return m_vLabels[hkFind(x)] = hkFind(y);
}

int ConnectedComponentAlgorithm::hkMakeSet()
{
    m_vLabels[0]++;
    m_vLabels[m_vLabels[0]] = m_vLabels[0];
    return m_vLabels[0];
}

void ConnectedComponentAlgorithm::hkInitialize(int max_labels)
{
    m_vLabels.resize(max_labels);
    m_vLabels[0] = 0;
}

void ConnectedComponentAlgorithm::hoshenKopelman(QVector<QVector<int>>& matrix_in)
{
    m_vLabels.clear();
    m_vMatrixOut = m_vMatrixIn;

    if (matrix_in.isEmpty() || matrix_in[0].isEmpty())
        return;

    int m = matrix_in[0].size();
    int n = matrix_in.size();

    hkInitialize(m * n / 2);

    for (int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (matrix_in[i][j])
            {

                int up = (i==0 ? 0 : matrix_in[i-1][j]);
                int left = (j==0 ? 0 : matrix_in[i][j-1]);

                switch (!!up + !!left)
                {

                case 0:
                    matrix_in[i][j] = hkMakeSet();
                    break;

                case 1:
                    matrix_in[i][j] = qMax(up,left);
                    break;

                case 2:
                    matrix_in[i][j] = hkUnion(up, left);
                    break;
                }
            }
        }
    }

    QVector<int> new_labels(m_vLabels.size());

    for (int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (matrix_in[i][j])
            {
                int x = hkFind(matrix_in[i][j]);

                if (new_labels[x] == 0)
                {
                    new_labels[0]++;
                    new_labels[x] = new_labels[0];
                }

                matrix_in[i][j] = new_labels[x];
            }
        }
    }

    m_vLabels = new_labels;
}


ConnectedComponentAlgorithm::ConnectedComponentAlgorithm(QVector<QVector<int> > matrix_in) :
    m_bComputed(false),
    m_vMatrixIn(matrix_in)
{

}

void ConnectedComponentAlgorithm::getOut(QVector<QVector<int> > &matrix, QVector<int>& labels)
{
    if (!m_bComputed)
    {
        hoshenKopelman(m_vMatrixOut);
        m_bComputed = true;
    }

    matrix = m_vMatrixOut;
    labels = m_vLabels;
}
