//@HEADER
/*
*******************************************************************************

   Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
   Copyright (C) 2010 EPFL, Politecnico di Milano, Emory UNiversity

   This file is part of the LifeV library

   LifeV is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   LifeV is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>


*******************************************************************************
*/
//@HEADER

/*!
 *   @file
     @brief This file contains the definition of the GraphElement class.

     @date 06/2011
     @author Radu Popescu <radu.popescu@epfl.ch>
 */

#ifndef GRAPH_ELEMENT_HPP
#define GRAPH_ELEMENT_HPP

#include <lifev/core/LifeV.hpp>

#include <lifev/core/fem/QuadratureRule.hpp>
#include <lifev/eta/fem/ETCurrentFE.hpp>
#include <lifev/eta/fem/MeshGeometricMap.hpp>

#include <lifev/eta/expression/ExpressionToEvaluation.hpp>

#include <lifev/eta/array/ETMatrixElemental.hpp>

#include <boost/shared_ptr.hpp>



namespace LifeV
{

namespace ExpressionAssembly
{


//! The class to actually perform the loop over the elements to precompute a graph
/*!
  @author Radu Popescu <radu.popescu@epfl.ch>

  This class is used to store the data required for building the graph of a
  matrix
 */
template < typename MeshType,
         typename TestSpaceType,
         typename SolutionSpaceType,
         typename ExpressionType >
class GraphElement
{
public:

    //! @name Public Types
    //@{

    //! Type of the Evaluation
    typedef typename ExpressionToEvaluation < ExpressionType,
            TestSpaceType::S_fieldDim,
            SolutionSpaceType::S_fieldDim,
            3 >::evaluation_Type  evaluation_Type;

    //@}


    //! @name Constructors, destructor
    //@{

    //! Full data constructor
    GraphElement (const boost::shared_ptr<MeshType>& mesh,
                  const QuadratureRule& quadrature,
                  const boost::shared_ptr<TestSpaceType>& testSpace,
                  const boost::shared_ptr<SolutionSpaceType>& solutionSpace,
                  const ExpressionType& expression,
                  const UInt offsetUp = 0,
                  const UInt offsetLeft = 0);

    //! Copy constructor
    GraphElement (const GraphElement < MeshType,
                  TestSpaceType,
                  SolutionSpaceType,
                  ExpressionType > & integrator);

    //! Destructor
    ~GraphElement();

    //@}


    //! @name Operators
    //@{

    //! Operator wrapping the addTo method (for shared_ptr)
    template <typename GraphType>
    inline void operator>> (boost::shared_ptr<GraphType> graph)
    {
        addTo (graph);
    }

    //@}


    //! @name Methods
    //@{

    //! Ouput method
    void check (std::ostream& out = std::cout);

    //! Method that builds the graph
    /*!
      The loop over the elements is located right
      in this method.
     */
    template <typename GraphType>
    void addTo (GraphType& graph);

    //! Method that performs the assembly
    /*!
      The loop over the elements is located right
      in this method.

      Specialized for the case where the matrix is passed as a shared_ptr
     */
    template <typename GraphType>
    inline void addTo (boost::shared_ptr<GraphType> graph)
    {
        ASSERT (graph != 0, " Cannot assemble with an empty graph");
        addTo (*graph);
    }

    //@}

private:

    //! @name Private Methods
    //@{

    //! No empty constructor
    GraphElement();

    //@}

    // Pointer on the mesh
    boost::shared_ptr<MeshType> M_mesh;

    // Quadrature to be used
    QuadratureRule M_quadrature;

    // Shared pointer on the Spaces
    boost::shared_ptr<TestSpaceType> M_testSpace;
    boost::shared_ptr<SolutionSpaceType> M_solutionSpace;

    // Offsets
    UInt M_offsetUp;
    UInt M_offsetLeft;
};


// ===================================================
// IMPLEMENTATION
// ===================================================

// ===================================================
// Constructors & Destructor
// ===================================================

template < typename MeshType, typename TestSpaceType, typename SolutionSpaceType, typename ExpressionType>
GraphElement<MeshType, TestSpaceType, SolutionSpaceType, ExpressionType>::
GraphElement (const boost::shared_ptr<MeshType>& mesh,
              const QuadratureRule& quadrature,
              const boost::shared_ptr<TestSpaceType>& testSpace,
              const boost::shared_ptr<SolutionSpaceType>& solutionSpace,
              const ExpressionType& /*expression*/,
              const UInt offsetUp,
              const UInt offsetLeft)
    :   M_mesh (mesh),
        M_quadrature (quadrature),
        M_testSpace (testSpace),
        M_solutionSpace (solutionSpace),
        M_offsetUp (offsetUp),
        M_offsetLeft (offsetLeft)
{
}

template < typename MeshType, typename TestSpaceType, typename SolutionSpaceType, typename ExpressionType>
GraphElement<MeshType, TestSpaceType, SolutionSpaceType, ExpressionType>::
GraphElement (const GraphElement<MeshType, TestSpaceType, SolutionSpaceType, ExpressionType>& integrator)
    :   M_mesh (integrator.M_mesh),
        M_quadrature (integrator.M_quadrature),
        M_testSpace (integrator.M_testSpace),
        M_solutionSpace (integrator.M_solutionSpace),
        M_offsetUp (integrator.M_offsetUp),
        M_offsetLeft (integrator.M_offsetLeft)
{
}

template < typename MeshType, typename TestSpaceType, typename SolutionSpaceType, typename ExpressionType>
GraphElement<MeshType, TestSpaceType, SolutionSpaceType, ExpressionType>::
~GraphElement()
{
}

// ===================================================
// Methods
// ===================================================

template < typename MeshType, typename TestSpaceType, typename SolutionSpaceType, typename ExpressionType>
void
GraphElement<MeshType, TestSpaceType, SolutionSpaceType, ExpressionType>::
check (std::ostream& out)
{
}


template < typename MeshType, typename TestSpaceType, typename SolutionSpaceType, typename ExpressionType>
template <typename GraphType>
void
GraphElement<MeshType, TestSpaceType, SolutionSpaceType, ExpressionType>::
addTo (GraphType& graph)
{
    UInt nbElements (M_mesh->numElements() );
    UInt nbTestDof (M_testSpace->refFE().nbDof() );
    UInt nbSolutionDof (M_solutionSpace->refFE().nbDof() );

    ETMatrixElemental elementalMatrix (TestSpaceType::S_fieldDim * M_testSpace->refFE().nbDof(),
                                       SolutionSpaceType::S_fieldDim * M_solutionSpace->refFE().nbDof() );

    for (UInt iElement = 0; iElement < nbElements; ++iElement)
    {
        // Zeros out the matrix
        elementalMatrix.zero();

        // Loop on the blocks
        for (UInt iblock (0); iblock < TestSpaceType::S_fieldDim; ++iblock)
        {
            for (UInt jblock (0); jblock < SolutionSpaceType::S_fieldDim; ++jblock)
            {

                // Set the row global indices in the local matrix
                for (UInt i (0); i < nbTestDof; ++i)
                {
                    elementalMatrix.setRowIndex
                    (i + iblock * nbTestDof,
                     M_testSpace->dof().localToGlobalMap (iElement, i) + iblock * M_testSpace->dof().numTotalDof() + M_offsetUp);
                }

                // Set the column global indices in the local matrix
                for (UInt j (0); j < nbSolutionDof; ++j)
                {
                    elementalMatrix.setColumnIndex
                    (j + jblock * nbSolutionDof,
                     M_solutionSpace->dof().localToGlobalMap (iElement, j) + jblock * M_solutionSpace->dof().numTotalDof() + M_offsetLeft);
                }
            }
        }
        const std::vector<Int>& rowIdx = elementalMatrix.rowIndices();
        const std::vector<Int>& colIdx = elementalMatrix.columnIndices();

        graph.InsertGlobalIndices (rowIdx.size(), &rowIdx[0],
                                   colIdx.size(), &colIdx[0]);
    }
}


} // Namespace ExpressionAssembly

} // Namespace LifeV

#endif