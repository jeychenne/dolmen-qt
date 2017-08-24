#ifndef BROWSERELEMENT_H
#define BROWSERELEMENT_H

// Abstract base class for all items that can be displayed in the left
// sidebar's tabs other than the corpus

class IBrowserElement
{
public:

    IBrowserElement();

    virtual ~IBrowserElement() = default;
    virtual void open() = 0;

    int id() const;

protected:
    int m_id;
};

#endif // BROWSERELEMENT_H
