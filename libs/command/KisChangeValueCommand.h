#ifndef KISCHANGEVALUECOMMAND_H
#define KISCHANGEVALUECOMMAND_H

#include "kundo2command.h"

template <auto mem_ptr, typename ValueType>
struct KisChangeValueCommand {};

template <typename Base, typename Member, Member Base::* mem_ptr, typename ValueType>
struct KisChangeValueCommand<mem_ptr, ValueType> : KUndo2Command
{
    KisChangeValueCommand(Base *base, const ValueType &newValue, KUndo2Command *parent = nullptr)
        : KUndo2Command(parent)
        , m_base(base)
        , m_oldValue(m_base->*mem_ptr)
        , m_newValue(newValue)
    {
    }

    void redo() {
        KUndo2Command::redo();
        m_base->*mem_ptr = m_newValue;
    }

    void undo() {
        m_base->*mem_ptr = m_oldValue;
        KUndo2Command::undo();
    }

protected:
    Base *m_base;
    ValueType m_oldValue;
    ValueType m_newValue;
};

template <auto mem_ptr, typename Base, typename ValueType>
KisChangeValueCommand<mem_ptr, ValueType> *makeChangeValueCommand(Base *base, const ValueType &newValue, KUndo2Command *parent = nullptr)
{
    return new KisChangeValueCommand<mem_ptr, ValueType>(base, newValue, parent);
}

template <auto mem_ptr, typename ValueType>
struct KisChangeIndirectValueCommand {};

template <typename Base, typename Member, Member Base::* mem_ptr, typename ValueType>
struct KisChangeIndirectValueCommand<mem_ptr, ValueType> : KUndo2Command
{
    KisChangeIndirectValueCommand(Base *base, const ValueType &newValue, KUndo2Command *parent = nullptr)
        : KUndo2Command(parent)
        , m_base(base)
        , m_oldValue(*(m_base->*mem_ptr))
        , m_newValue(newValue)
    {
    }

    void redo() {
        KUndo2Command::redo();
        *(m_base->*mem_ptr) = m_newValue;
    }

    void undo() {
        *(m_base->*mem_ptr) = m_oldValue;
        KUndo2Command::undo();
    }
private:
    Base *m_base;
    ValueType m_oldValue;
    ValueType m_newValue;
};

template <auto mem_ptr, typename Base, typename ValueType>
KisChangeIndirectValueCommand<mem_ptr, ValueType>* makeChangeIndirectValueCommand(Base *base, const ValueType &newValue, KUndo2Command *parent = nullptr)
{
    return new KisChangeIndirectValueCommand<mem_ptr, ValueType>(base, newValue, parent);
}

#endif
