/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QV4CODEGEN_P_H
#define QV4CODEGEN_P_H

#include "private/qv4global_p.h"
#include "qv4jsir_p.h"
#include <private/qqmljsastvisitor_p.h>
#include <private/qqmljsast_p.h>
#include <QtCore/QStringList>
#include <QStack>
#include <qqmlerror.h>
#include <assert.h>
#include <private/qv4util_p.h>

QT_BEGIN_NAMESPACE

namespace QV4 {
struct ExecutionContext;
}

namespace QQmlJS {
namespace AST {
class UiParameterList;
}


class Q_QML_EXPORT Codegen: protected AST::Visitor
{
public:
    Codegen(bool strict);

    enum CompilationMode {
        GlobalCode,
        EvalCode,
        FunctionCode,
        QmlBinding // This is almost the same as EvalCode, except:
                   //  * function declarations are moved to the return address when encountered
                   //  * return statements are allowed everywhere (like in FunctionCode)
                   //  * variable declarations are treated as true locals (like in FunctionCode)
    };

    void generateFromProgram(const QString &fileName,
                             const QString &sourceCode,
                             AST::Program *ast,
                             V4IR::Module *module,
                             CompilationMode mode = GlobalCode,
                             const QStringList &inheritedLocals = QStringList());
    void generateFromFunctionExpression(const QString &fileName,
                             const QString &sourceCode,
                             AST::FunctionExpression *ast,
                             V4IR::Module *module);

protected:
    enum Format { ex, cx, nx };
    struct Result {
        V4IR::Expr *code;
        V4IR::BasicBlock *iftrue;
        V4IR::BasicBlock *iffalse;
        Format format;
        Format requested;

        explicit Result(Format requested = ex)
            : code(0)
            , iftrue(0)
            , iffalse(0)
            , format(ex)
            , requested(requested) {}

        explicit Result(V4IR::BasicBlock *iftrue, V4IR::BasicBlock *iffalse)
            : code(0)
            , iftrue(iftrue)
            , iffalse(iffalse)
            , format(ex)
            , requested(cx) {}

        inline V4IR::Expr *operator*() const { Q_ASSERT(format == ex); return code; }
        inline V4IR::Expr *operator->() const { Q_ASSERT(format == ex); return code; }

        bool accept(Format f)
        {
            if (requested == f) {
                format = f;
                return true;
            }
            return false;
        }
    };

    struct Environment {
        Environment *parent;

        enum MemberType {
            UndefinedMember,
            VariableDefinition,
            VariableDeclaration,
            FunctionDefinition
        };
        struct Member {
            MemberType type;
            int index;
            AST::FunctionExpression *function;
        };
        typedef QMap<QString, Member> MemberMap;

        MemberMap members;
        AST::FormalParameterList *formals;
        int maxNumberOfArguments;
        bool hasDirectEval;
        bool hasNestedFunctions;
        bool isStrict;
        bool isNamedFunctionExpression;
        bool usesThis;
        enum UsesArgumentsObject {
            ArgumentsObjectUnknown,
            ArgumentsObjectNotUsed,
            ArgumentsObjectUsed
        };

        UsesArgumentsObject usesArgumentsObject;

        CompilationMode compilationMode;

        Environment(Environment *parent, CompilationMode mode)
            : parent(parent)
            , formals(0)
            , maxNumberOfArguments(0)
            , hasDirectEval(false)
            , hasNestedFunctions(false)
            , isStrict(false)
            , isNamedFunctionExpression(false)
            , usesThis(false)
            , usesArgumentsObject(ArgumentsObjectUnknown)
            , compilationMode(mode)
        {
            if (parent && parent->isStrict)
                isStrict = true;
        }

        int findMember(const QString &name) const
        {
            MemberMap::const_iterator it = members.find(name);
            if (it == members.end())
                return -1;
            assert((*it).index != -1 || !parent);
            return (*it).index;
        }

        bool lookupMember(const QString &name, Environment **scope, int *index, int *distance)
        {
            Environment *it = this;
            *distance = 0;
            for (; it; it = it->parent, ++(*distance)) {
                int idx = it->findMember(name);
                if (idx != -1) {
                    *scope = it;
                    *index = idx;
                    return true;
                }
            }
            return false;
        }

        void enter(const QString &name, MemberType type, AST::FunctionExpression *function = 0)
        {
            if (! name.isEmpty()) {
                if (type != FunctionDefinition) {
                    for (AST::FormalParameterList *it = formals; it; it = it->next)
                        if (it->name == name)
                            return;
                }
                MemberMap::iterator it = members.find(name);
                if (it == members.end()) {
                    Member m;
                    m.index = -1;
                    m.type = type;
                    m.function = function;
                    members.insert(name, m);
                } else {
                    if ((*it).type <= type) {
                        (*it).type = type;
                        (*it).function = function;
                    }
                }
            }
        }
    };

    Environment *newEnvironment(AST::Node *node, Environment *parent, CompilationMode compilationMode)
    {
        Environment *env = new Environment(parent, compilationMode);
        _envMap.insert(node, env);
        return env;
    }

    struct UiMember {
    };

    struct ScopeAndFinally {
        enum ScopeType {
            WithScope,
            TryScope,
            CatchScope
        };

        ScopeAndFinally *parent;
        AST::Finally *finally;
        ScopeType type;

        ScopeAndFinally(ScopeAndFinally *parent, ScopeType t = WithScope) : parent(parent), finally(0), type(t) {}
        ScopeAndFinally(ScopeAndFinally *parent, AST::Finally *finally)
        : parent(parent), finally(finally), type(TryScope)
        {}
    };

    struct Loop {
        AST::LabelledStatement *labelledStatement;
        AST::Statement *node;
        V4IR::BasicBlock *groupStartBlock;
        V4IR::BasicBlock *breakBlock;
        V4IR::BasicBlock *continueBlock;
        Loop *parent;
        ScopeAndFinally *scopeAndFinally;

        Loop(AST::Statement *node, V4IR::BasicBlock *groupStartBlock, V4IR::BasicBlock *breakBlock, V4IR::BasicBlock *continueBlock, Loop *parent)
            : labelledStatement(0), node(node), groupStartBlock(groupStartBlock), breakBlock(breakBlock), continueBlock(continueBlock), parent(parent) {}
    };

    void enterEnvironment(AST::Node *node);
    void leaveEnvironment();

    void enterLoop(AST::Statement *node, V4IR::BasicBlock *startBlock, V4IR::BasicBlock *breakBlock, V4IR::BasicBlock *continueBlock);
    void leaveLoop();
    V4IR::BasicBlock *groupStartBlock() const
    {
        for (Loop *it = _loop; it; it = it->parent)
            if (it->groupStartBlock)
                return it->groupStartBlock;
        return 0;
    }
    V4IR::BasicBlock *exceptionHandler() const
    {
        if (_exceptionHandlers.isEmpty())
            return 0;
        return _exceptionHandlers.top();
    }
    void pushExceptionHandler(V4IR::BasicBlock *handler)
    {
        handler->isExceptionHandler = true;
        _exceptionHandlers.push(handler);
    }
    void popExceptionHandler()
    {
        Q_ASSERT(!_exceptionHandlers.isEmpty());
        _exceptionHandlers.pop();
    }

    virtual V4IR::Expr *member(V4IR::Expr *base, const QString *name); // Re-implemented by QML to resolve QObject property members
    V4IR::Expr *subscript(V4IR::Expr *base, V4IR::Expr *index);
    V4IR::Expr *argument(V4IR::Expr *expr);
    V4IR::Expr *reference(V4IR::Expr *expr);
    V4IR::Expr *unop(V4IR::AluOp op, V4IR::Expr *expr);
    V4IR::Expr *binop(V4IR::AluOp op, V4IR::Expr *left, V4IR::Expr *right);
    V4IR::Expr *call(V4IR::Expr *base, V4IR::ExprList *args);
    void move(V4IR::Expr *target, V4IR::Expr *source, V4IR::AluOp op = V4IR::OpInvalid);
    void cjump(V4IR::Expr *cond, V4IR::BasicBlock *iftrue, V4IR::BasicBlock *iffalse);

    // Returns index in _module->functions
    int defineFunction(const QString &name, AST::Node *ast,
                       AST::FormalParameterList *formals,
                       AST::SourceElements *body,
                       const QStringList &inheritedLocals = QStringList());

    void unwindException(ScopeAndFinally *outest);

    void statement(AST::Statement *ast);
    void statement(AST::ExpressionNode *ast);
    void condition(AST::ExpressionNode *ast, V4IR::BasicBlock *iftrue, V4IR::BasicBlock *iffalse);
    Result expression(AST::ExpressionNode *ast);
    QString propertyName(AST::PropertyName *ast);
    Result sourceElement(AST::SourceElement *ast);
    UiMember uiObjectMember(AST::UiObjectMember *ast);

    void accept(AST::Node *node);

    void functionBody(AST::FunctionBody *ast);
    void program(AST::Program *ast);
    void sourceElements(AST::SourceElements *ast);
    void variableDeclaration(AST::VariableDeclaration *ast);
    void variableDeclarationList(AST::VariableDeclarationList *ast);

    V4IR::Expr *identifier(const QString &name, int line = 0, int col = 0);
    // Hook provided to implement QML lookup semantics
    virtual V4IR::Expr *fallbackNameLookup(const QString &name, int line, int col);

    // nodes
    virtual bool visit(AST::ArgumentList *ast);
    virtual bool visit(AST::CaseBlock *ast);
    virtual bool visit(AST::CaseClause *ast);
    virtual bool visit(AST::CaseClauses *ast);
    virtual bool visit(AST::Catch *ast);
    virtual bool visit(AST::DefaultClause *ast);
    virtual bool visit(AST::ElementList *ast);
    virtual bool visit(AST::Elision *ast);
    virtual bool visit(AST::Finally *ast);
    virtual bool visit(AST::FormalParameterList *ast);
    virtual bool visit(AST::FunctionBody *ast);
    virtual bool visit(AST::Program *ast);
    virtual bool visit(AST::PropertyNameAndValue *ast);
    virtual bool visit(AST::PropertyAssignmentList *ast);
    virtual bool visit(AST::PropertyGetterSetter *ast);
    virtual bool visit(AST::SourceElements *ast);
    virtual bool visit(AST::StatementList *ast);
    virtual bool visit(AST::UiArrayMemberList *ast);
    virtual bool visit(AST::UiImport *ast);
    virtual bool visit(AST::UiHeaderItemList *ast);
    virtual bool visit(AST::UiPragma *ast);
    virtual bool visit(AST::UiObjectInitializer *ast);
    virtual bool visit(AST::UiObjectMemberList *ast);
    virtual bool visit(AST::UiParameterList *ast);
    virtual bool visit(AST::UiProgram *ast);
    virtual bool visit(AST::UiQualifiedId *ast);
    virtual bool visit(AST::UiQualifiedPragmaId *ast);
    virtual bool visit(AST::VariableDeclaration *ast);
    virtual bool visit(AST::VariableDeclarationList *ast);

    // expressions
    virtual bool visit(AST::Expression *ast);
    virtual bool visit(AST::ArrayLiteral *ast);
    virtual bool visit(AST::ArrayMemberExpression *ast);
    virtual bool visit(AST::BinaryExpression *ast);
    virtual bool visit(AST::CallExpression *ast);
    virtual bool visit(AST::ConditionalExpression *ast);
    virtual bool visit(AST::DeleteExpression *ast);
    virtual bool visit(AST::FalseLiteral *ast);
    virtual bool visit(AST::FieldMemberExpression *ast);
    virtual bool visit(AST::FunctionExpression *ast);
    virtual bool visit(AST::IdentifierExpression *ast);
    virtual bool visit(AST::NestedExpression *ast);
    virtual bool visit(AST::NewExpression *ast);
    virtual bool visit(AST::NewMemberExpression *ast);
    virtual bool visit(AST::NotExpression *ast);
    virtual bool visit(AST::NullExpression *ast);
    virtual bool visit(AST::NumericLiteral *ast);
    virtual bool visit(AST::ObjectLiteral *ast);
    virtual bool visit(AST::PostDecrementExpression *ast);
    virtual bool visit(AST::PostIncrementExpression *ast);
    virtual bool visit(AST::PreDecrementExpression *ast);
    virtual bool visit(AST::PreIncrementExpression *ast);
    virtual bool visit(AST::RegExpLiteral *ast);
    virtual bool visit(AST::StringLiteral *ast);
    virtual bool visit(AST::ThisExpression *ast);
    virtual bool visit(AST::TildeExpression *ast);
    virtual bool visit(AST::TrueLiteral *ast);
    virtual bool visit(AST::TypeOfExpression *ast);
    virtual bool visit(AST::UnaryMinusExpression *ast);
    virtual bool visit(AST::UnaryPlusExpression *ast);
    virtual bool visit(AST::VoidExpression *ast);
    virtual bool visit(AST::FunctionDeclaration *ast);

    // property names
    virtual bool visit(AST::IdentifierPropertyName *ast);
    virtual bool visit(AST::NumericLiteralPropertyName *ast);
    virtual bool visit(AST::StringLiteralPropertyName *ast);

    // source elements
    virtual bool visit(AST::FunctionSourceElement *ast);
    virtual bool visit(AST::StatementSourceElement *ast);

    // statements
    virtual bool visit(AST::Block *ast);
    virtual bool visit(AST::BreakStatement *ast);
    virtual bool visit(AST::ContinueStatement *ast);
    virtual bool visit(AST::DebuggerStatement *ast);
    virtual bool visit(AST::DoWhileStatement *ast);
    virtual bool visit(AST::EmptyStatement *ast);
    virtual bool visit(AST::ExpressionStatement *ast);
    virtual bool visit(AST::ForEachStatement *ast);
    virtual bool visit(AST::ForStatement *ast);
    virtual bool visit(AST::IfStatement *ast);
    virtual bool visit(AST::LabelledStatement *ast);
    virtual bool visit(AST::LocalForEachStatement *ast);
    virtual bool visit(AST::LocalForStatement *ast);
    virtual bool visit(AST::ReturnStatement *ast);
    virtual bool visit(AST::SwitchStatement *ast);
    virtual bool visit(AST::ThrowStatement *ast);
    virtual bool visit(AST::TryStatement *ast);
    virtual bool visit(AST::VariableStatement *ast);
    virtual bool visit(AST::WhileStatement *ast);
    virtual bool visit(AST::WithStatement *ast);

    // ui object members
    virtual bool visit(AST::UiArrayBinding *ast);
    virtual bool visit(AST::UiObjectBinding *ast);
    virtual bool visit(AST::UiObjectDefinition *ast);
    virtual bool visit(AST::UiPublicMember *ast);
    virtual bool visit(AST::UiScriptBinding *ast);
    virtual bool visit(AST::UiSourceElement *ast);

    bool throwSyntaxErrorOnEvalOrArgumentsInStrictMode(V4IR::Expr* expr, const AST::SourceLocation &loc);
    virtual void throwSyntaxError(const AST::SourceLocation &loc, const QString &detail);
    virtual void throwReferenceError(const AST::SourceLocation &loc, const QString &detail);

public:
    QList<QQmlError> errors() const;

protected:
    Result _expr;
    QString _property;
    UiMember _uiMember;
    V4IR::Module *_module;
    V4IR::Function *_function;
    V4IR::BasicBlock *_block;
    V4IR::BasicBlock *_exitBlock;
    unsigned _returnAddress;
    Environment *_env;
    Loop *_loop;
    AST::LabelledStatement *_labelledStatement;
    ScopeAndFinally *_scopeAndFinally;
    QHash<AST::Node *, Environment *> _envMap;
    QHash<AST::FunctionExpression *, int> _functionMap;
    QStack<V4IR::BasicBlock *> _exceptionHandlers;
    bool _strictMode;

    bool _fileNameIsUrl;
    bool hasError;
    QList<QQmlError> _errors;

    class ScanFunctions: protected Visitor
    {
        typedef QV4::TemporaryAssignment<bool> TemporaryBoolAssignment;
    public:
        ScanFunctions(Codegen *cg, const QString &sourceCode, CompilationMode defaultProgramMode);
        void operator()(AST::Node *node);

        void enterEnvironment(AST::Node *node, CompilationMode compilationMode);
        void leaveEnvironment();

        void enterQmlScope(AST::Node *ast, const QString &name)
        { enterFunction(ast, name, /*formals*/0, /*body*/0, /*expr*/0, /*isExpression*/false); }

        void enterQmlFunction(AST::FunctionDeclaration *ast)
        { enterFunction(ast, false, false); }

    protected:
        using Visitor::visit;
        using Visitor::endVisit;

        void checkDirectivePrologue(AST::SourceElements *ast);

        void checkName(const QStringRef &name, const AST::SourceLocation &loc);
        void checkForArguments(AST::FormalParameterList *parameters);

        virtual bool visit(AST::Program *ast);
        virtual void endVisit(AST::Program *);

        virtual bool visit(AST::CallExpression *ast);
        virtual bool visit(AST::NewMemberExpression *ast);
        virtual bool visit(AST::ArrayLiteral *ast);
        virtual bool visit(AST::VariableDeclaration *ast);
        virtual bool visit(AST::IdentifierExpression *ast);
        virtual bool visit(AST::ExpressionStatement *ast);
        virtual bool visit(AST::FunctionExpression *ast);

        void enterFunction(AST::FunctionExpression *ast, bool enterName, bool isExpression = true);

        virtual void endVisit(AST::FunctionExpression *);

        virtual bool visit(AST::ObjectLiteral *ast);

        virtual bool visit(AST::PropertyGetterSetter *ast);
        virtual void endVisit(AST::PropertyGetterSetter *);

        virtual bool visit(AST::FunctionDeclaration *ast);
        virtual void endVisit(AST::FunctionDeclaration *);

        virtual bool visit(AST::WithStatement *ast);

        virtual bool visit(AST::DoWhileStatement *ast);
        virtual bool visit(AST::ForStatement *ast);
        virtual bool visit(AST::LocalForStatement *ast);
        virtual bool visit(AST::ForEachStatement *ast);
        virtual bool visit(AST::LocalForEachStatement *ast);
        virtual bool visit(AST::ThisExpression *ast);

        virtual bool visit(AST::Block *ast);

    protected:
        void enterFunction(AST::Node *ast, const QString &name, AST::FormalParameterList *formals, AST::FunctionBody *body, AST::FunctionExpression *expr, bool isExpression);

    // fields:
        Codegen *_cg;
        const QString _sourceCode;
        Environment *_env;
        QStack<Environment *> _envStack;

        bool _allowFuncDecls;
        CompilationMode defaultProgramMode;
    };

};

class RuntimeCodegen : public Codegen
{
public:
    RuntimeCodegen(QV4::ExecutionContext *ctx, bool strict)
        : Codegen(strict)
        , context(ctx)
    {}

    virtual void throwSyntaxError(const AST::SourceLocation &loc, const QString &detail);
    virtual void throwReferenceError(const AST::SourceLocation &loc, const QString &detail);
private:
    QV4::ExecutionContext *context;
};

}

QT_END_NAMESPACE

#endif // QV4CODEGEN_P_H
