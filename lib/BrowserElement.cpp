#include "BrowserElement.h"
#include "utils/routines.h"

IBrowserElement::IBrowserElement()
{
    m_id = genElementId();
}

int IBrowserElement::id() const
{
    return m_id;
}
