/***************************************************************************
    elementmodel.h  -  Definition of ElementModeler class
                             -------------------
    begin                : November 19 2002
    copyright            : (C) 2003 by Vojtìch Toman
    email                : vtoman@lit.cz
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*!
  \file elementmodel.h
  \brief Definitions of element modeling related classes and structures.

  This file contains definitions of classes and structures needed for the modeling of the elements.
*/

#ifdef __GNUG__
# pragma interface
#endif


#ifndef ELEMENTMODEL_H
#define ELEMENTMODEL_H

#include <cmath>

#include "defs.h"
#include "list.h"
#include "stack.h"
#include "xmlchar.h"
#include "saxemitter.h"


//! The string indicating the "start node" node of the element model.
#define STR_NODE_TYPE_START_NODE	"S"

//! The string indicating the "end node" node of the element model.
#define STR_NODE_TYPE_END_NODE		"/"

//! The string indicating the "element node" node of the element model.
#define STR_NODE_TYPE_ELEMENT_NODE	"E"

//! The string indicating the "characters node" node of the element model.
#define STR_NODE_TYPE_CHARACTERS_NODE	"C"


class ElementModeler;	//forward declaration


/*!
  \brief A namespace for element modeling structures.
 */
namespace ElementModel
{
  struct Node;		//forward declaration, see below


  //! Structure describing one graph transition.
  struct Transition
  {
    //! The frequency count.
    size_t frequency;

    //! The %id of the transition.
    size_t id;

    //! The end %node of the transition.
    Node *node;
  };


  //! The type representing a list of transitions.
  typedef List<Transition> Transitions;


  //! An enum describing possible graph node types.
  enum NodeType
  {
    //! No node.
    NoNode,

    //! The start node.
    StartNode,

    //! he end node.
    EndNode,

    //! The characters node.
    CharactersNode,

    //! The element mode (contains a reference to another graph).
    ElementNode,
  };


  //! The possible states of one transition.
  enum TransitionState
  {
    //! The transition is definite.
    Definite,

    //! The transition is indefinite (e.g. there are more possibilities).
    Indefinite,

    //! Leads to the node that has been created just now
    NewNodeCreated,

    //! There is no such transition.
    Impossible
  };


  //! One graph node.
  struct Node
  {
    //! The id of the node.
    unsigned long id;

    //! the type of the node.
    NodeType type;

    //! %List of outgoing edges.
    Transitions successors;

    //! Pointer to the most probable outgoing edge.
    Transition *mpt;

    //! Pointer to the modeler of the referenced element (used only for element nodes).
    ElementModeler *modeler;
  };

  /*! 
    \brief An attribute node.

    This node always follows the StartNode and indicates whether the element should have or shouldn't have some attributes.
  */
  struct AttributeNode : public Node
  {
    //! Number of appearances of the element without the attributes.
    size_t noAttrCnt;

    //! Number of appearances of the element with the attributes.
    size_t yesAttrCnt;

    //! The prediction whether the attributes are expected, or not.
    bool hasAttr;
  };


  //! The type representing a stack of nodes.
  typedef Stack<Node> NodeStack;
};



/*!
  \brief Element modeling class.

  This class models one element of the XML document. It maintains the element graph structure and performs the update operations, as well as the motion in the graph.
 */
class ElementModeler
{
public:
  //! A constructor.
  ElementModeler(XmlChar *name);

  //! A destructor.
  virtual ~ElementModeler(void);

  //! Set the position to the start node.
  virtual void resetCurrentNode(void);

  //! Pop the node from the node stack.
  virtual void popCurrentNode(void);

  //! Get the node from the node stack.
  virtual ElementModel::Node *getCurrentNode(void);

  //! Move to desired node.
  virtual ElementModel::TransitionState moveToDesiredNode(ElementModel::NodeType desiredNodeType, size_t *edgeId, size_t *elementPushes, ElementModeler *elementModeler = 0);

  //! Follow the deterministic edge.
  virtual ElementModel::NodeType moveForward(ElementModel::Node **node, SAXEmitter *saxEmitter, void *userData);

  //! Follow the given edge.
  virtual ElementModel::NodeType followEdge(size_t edgeId, ElementModel::Node **node, SAXEmitter *saxEmitter, void *userData);

  //! Get the type of the current node.
  virtual ElementModel::NodeType currentNodeType(void);

  //! Set the attribute flag.
  virtual bool setAttributes(bool attributes);

  //! Test for attributes.
  virtual bool hasAttributes(void);

  //! Get the start node of the model.
  virtual ElementModel::Node *getStartNode(void);

  //! Get the end node of the model.
  virtual ElementModel::Node *getEndNode(void);

  //! Get the name of the element.
  virtual XmlChar *getElementName(void);

  //! Increase the reference count.
  virtual void increaseRefCount(void);

  //! Get the reference count.
  virtual size_t getRefCount(void);

  //! Compute the structural entropy of the model.
  virtual void computeStructuralEntropy(void);

  //! Get the structural entropy of the model.
  virtual double getStructuralEntropy(void);

  //! Display the graph of the element.
  virtual void print(void);

  //! Display one node of the graph.
  virtual void printNode(ElementModel::Node *node);


protected:
  //! The name of the element.
  XmlChar *elementName;

  //! Pointer to the start node.
  ElementModel::AttributeNode *startNode;

  //! Pointer to the end node.
  ElementModel::Node *endNode;

  //! Pointer to the current node.
  ElementModel::Node *currentNode;

  //! The stack of nodes.
  ElementModel::NodeStack nodeStack;

  //! The counter of nodes.
  size_t nodeCounter;

  //! The reference count of the model.
  size_t refCount;

  //! The structural entropy of the model.
  double structuralEntropy;

  //! List of the nodes of the model.
  List<ElementModel::Node> nodeList;
};



//! A structure describing one element modeler.
struct ElementModelerInfo
{
  //! pointer to the modeler.
  ElementModeler *modeler;

  //! Indication whether the model is currently building new nodes.
  bool building;
};


//! A structure describing the probability of given transition in the element model.
struct ElementModelTransitionInfo
{
  //! The probability of the transition.
  double probability;

  //! The pointer to the transition.
  ElementModel::Transition *transition;
};


/*!
  \brief A static class for calculating the structural entropy of the models of the elements.

  The only one method of the class is the static method calculate() which calculates the structural entropy of the model.
 */
class ElementModelEntropyCalculator
{
 public:
  //! Calculate the structural entropy of given model.
  static double calculate(ElementModeler *modeler);
};



#endif //ELEMENTMODEL_H
