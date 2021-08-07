#include "dieview.h"

DieView::DieView(Die *die)
{
    m_die = die;

    makeUI();
}

DieView::~DieView()
{
    delete m_die;
    m_die = nullptr;
}

void DieView::makeUI()
{

}
