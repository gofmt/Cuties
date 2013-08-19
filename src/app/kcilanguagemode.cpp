#include "kcilanguagemode.h"

kciLanguageMode::kciLanguageMode(QWidget *parent) :
    QObject(parent)
{
    m_parent=qobject_cast<kciCodeEditor*>(parent);
    m_type=plainText;
    compilerReceiver=NULL;
    dbgReceiver=NULL;
    gdbInstance=NULL;

    setCompileState(uncompiled);

    Q_ASSERT(m_parent!=NULL);
}

void kciLanguageMode::setMode(const modeType &type)
{
    m_type=type;
}

void kciLanguageMode::compile()
{
    if(compiler.isNull())
    {
        qDebug()<<"compiler is NULL";
        return ;
    }

    if(compilerReceiver==NULL)
    {
        compilerReceiver=new compileOutputReceiver(this);
        connectCompilerAndOutputReceiver();
    }

    if(checkIfIsCompiling())
        return ;

    setCompileState(compiling);

    compilerReceiver->addForwardText();

    compilerFinishedConnection=connect(compiler.data(),&compilerBase::compileFinished,
                                       this,&kciLanguageMode::onCompileFinished);
    compiler->startCompile(m_parent->filePath);
}

void kciLanguageMode::setFileSuffix(const QString& suffix)
{
    QRegularExpression _regexp_cpp("(h|hpp|rh|hh|c|cpp|cc|cxx|c++|cp)",
                                   QRegularExpression::CaseInsensitiveOption);
    QRegularExpression _regexp_pascal("pas",
                                      QRegularExpression::CaseInsensitiveOption);

    if(suffix.contains(_regexp_cpp))
    {
        if(m_type==cpp) //file type doesn't change,so return.
            return ;

        m_type=cpp;
        compiler.reset(new gcc(this));
        m_highlighter.reset(new cppHighlighter(this));
    }
    else if(suffix.contains(_regexp_pascal))
    {
        if(m_type==pascal) //file type doesn't change,so return.
            return ;

        m_type=pascal;
        compiler.reset(new fpc(this));
        m_highlighter.reset(new pascalHighlighter(this));
    }
    else
    {
        if(m_type==plainText) //file type doesn't change,so return.
            return ;

        m_type=plainText;
        compiler.reset();
        m_highlighter.reset(new kciHighlighter(this));
    }


    Q_ASSERT(!m_highlighter.isNull());
    m_highlighter->setDocument(m_parent->document);
}

compileOutputReceiver* kciLanguageMode::getCompilerReceiver() const
{
    return compilerReceiver;
}

dbgOutputReceiver* kciLanguageMode::getDbgReceiver() const
{
    return dbgReceiver;
}

gdb* kciLanguageMode::getGdbInstance() const
{
    return gdbInstance;
}

gdb* kciLanguageMode::startDebug()
{
    if(gdbInstance == NULL)
        gdbInstance=new gdb(this);

    if(dbgReceiver == NULL)
        dbgReceiver=new dbgOutputReceiver(this);

    connectGDBAndDbgReceiver();
    gdbInstance->runGDB(m_parent->execFileName);

    return gdbInstance;
}

void kciLanguageMode::onCompileFinished(bool hasError)
{
    if((bool)compilerFinishedConnection)
        disconnect(compilerFinishedConnection);

    setCompileState(compiled);

    if(!hasError)
    {
        emit compileSuccessfully(m_parent->execFileName);
    }
}

void kciLanguageMode::connectCompilerAndOutputReceiver()
{
    compilerConnectionHandles.disConnectAll();

    compilerReceiver->setCompilerVersionString(compiler->compilerName()+
                                               " "+
                                               compiler->version());

    //Output Compile Message:
    compilerConnectionHandles+=connect(compiler.data(),&compilerBase::compileCmd,
                               compilerReceiver,&compileOutputReceiver::addText);
    compilerConnectionHandles+=connect(compiler.data(),&compilerBase::output,
                               compilerReceiver,&compileOutputReceiver::addText);
    compilerConnectionHandles+=connect(compiler.data(),&compilerBase::compileError,
                               compilerReceiver,&compileOutputReceiver::onCompileMsgReceived);
    compilerConnectionHandles+=connect(compiler.data(),&compilerBase::compileFinished,
                               compilerReceiver,&compileOutputReceiver::onCompileFinished);
}

void kciLanguageMode::connectGDBAndDbgReceiver()
{
    gdbConnectionHandles.disConnectAll();

    gdbConnectionHandles+=connect(gdbInstance,&gdb::errorOccured,
                               dbgReceiver,&dbgOutputReceiver::receiveError);
    gdbConnectionHandles+=connect(gdbInstance,&gdb::consoleOutputStream,
                               dbgReceiver,&dbgOutputReceiver::receiveconsoleOutput);
    gdbConnectionHandles+=connect(gdbInstance,&gdb::targetOutputStream,
                               dbgReceiver,&dbgOutputReceiver::receivetargetOutput);
    gdbConnectionHandles+=connect(gdbInstance,&gdb::logOutputStream,
                               dbgReceiver,&dbgOutputReceiver::receivelogOutput);
    gdbConnectionHandles+=connect(gdbInstance,&gdb::locals,
                               dbgReceiver,&dbgOutputReceiver::receiveLocals);
}

bool kciLanguageMode::checkIfIsCompiling()
{
    bool ret=false;

    stateLock.lockForRead();
    if(state==compiling)
        ret=true;
    stateLock.unlock();

    return ret;
}

void kciLanguageMode::setCompileState(const compileState &state)
{
    stateLock.lockForWrite();
    this->state=state;
    stateLock.unlock();
}
