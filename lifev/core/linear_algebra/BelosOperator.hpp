/*
 * BelosOperator.hpp
 *
 *  Created on: Sep 28, 2010
 *      Author: uvilla
 */

//@HEADER

/*!
 * \file BelosOperator.hpp
 * \author Umberto Villa
 * \date 2010-09-28
 * Interface to the Belos linear solver in Trilinos. This interface requires the user to provide
 * the preconditioner in the form of a LinearOperator.
 */

#ifndef BELOS_OPERATOR_HPP_
#define BELOS_OPERATOR_HPP_

#include <BelosEpetraAdapter.hpp>
#include <BelosSolverManager.hpp>
#include <Teuchos_ParameterList.hpp>

#include <lifev/core/linear_algebra/InvertibleOperator.hpp>

namespace LifeV
{
namespace Operators
{
//! @class
/*! @brief InvertibleOperator interface to Belos in Trilinos.
 * BelosOperator requires the operator to be solved and the preconditioner in the form of LinearOperator.
 *
 * For a description of the class functionality, please refer to the parent class InvertibleOperator.
 *
 */

class BelosOperator : public InvertibleOperator
{
public:

    //! @name Public Typedefs and Enumerators
    //@{

    enum PreconditionerSide{None, Left, Right};

    enum SolverManagerType { NotAValidSolverManager, BlockCG, PseudoBlockCG, RCG,
                             BlockGmres, PseudoBlockGmres, GmresPoly,
                             GCRODR, PCPG, Minres, TFQMR };

    typedef std::map<std::string, SolverManagerType> solverManagerMap_Type;
    typedef std::map<std::string, PreconditionerSide> precSideMap_Type;
    typedef Epetra_MultiVector MV;
    typedef Epetra_Operator    OP;
    typedef Belos::LinearProblem<double,MV,OP> LinearProblem;
    typedef Belos::SolverManager<double,MV,OP> SolverType;
    typedef Teuchos::RCP<LinearProblem> LinearProblem_ptr;
    typedef Teuchos::RCP<SolverType>    SolverType_ptr;

    //@}

    //! null constructor and destructor
    //@{
    BelosOperator();
    ~BelosOperator() {};
    //@}
    static solverManagerMap_Type * singletonSolverManagerMap();
    static precSideMap_Type      * singletonPrecSideMap();

protected:

    virtual int doApplyInverse(const vector_Type& X, vector_Type& Y) const;
    virtual void doSetOperator();
    virtual void doSetPreconditioner();
    virtual void doSetParameterList();
    void allocateSolver(const SolverManagerType & solverManagerType);
    //! The linearProblem
    LinearProblem_ptr M_linProblem;
    //! The linearSolver
    SolverType_ptr M_solverManager;
    //! Cast to a Belos Preconditioner
    Teuchos::RCP<Belos::EpetraPrecOp> M_belosPrec;
    static std::auto_ptr< solverManagerMap_Type > S_solverManagerMap;
    static std::auto_ptr<precSideMap_Type> S_precSideMap;
};

inline InvertibleOperator* createBelosOperator() { return new BelosOperator(); }
namespace
{
    static bool registerBelos = InvertibleOperatorFactory::instance().registerProduct( "Belos", &createBelosOperator );
}


} /*end namespace Operators */
} /*end namespace */
#endif /* BELOS_OPERATOR_HPP_ */