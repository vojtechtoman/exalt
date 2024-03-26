/***************************************************************************
    elementmodel.cpp  -  Definition of ElementModeler class methods
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
  \file elementmodel.cpp
  \brief Definitions of ElementModeler class methods.

  This file contains definitions of ElementModeler class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif



#include "elementmodel.h"


/*!
  \brief Increases the frequency count for given transition.

  \param _node_ The pointer to the graph node.
  \param _tr_ The pointer to the transition.
*/
#define INC_TRANSITION_FREQUENCY(_node_, _tr_)		\
{							\
  _tr_->frequency++;					\
  ElementModel::Node *_tmpnode_ = _node_;		\
							\
  if (_tmpnode_->mpt == _tr_)				\
    {							\
    }							\
  else							\
    if (!_tmpnode_->mpt)				\
      {							\
        _tmpnode_->mpt = _tr_;				\
      }							\
    else						\
      if (_tr_->frequency == _tmpnode_->mpt->frequency)	\
        {						\
          _tmpnode_->mpt = _tr_;			\
        }						\
      else						\
        if (_tr_->frequency > _tmpnode_->mpt->frequency)\
	  {						\
            _tmpnode_->mpt = _tr_;			\
          }						\
}



/*!
  \brief Creates a transition bewteen two nodes.

  \param _from_ The start node.
  \param _to_ The end node.
*/
#define CREATE_GRAPH_TRANSITION(_from_, _to_)				\
{									\
  ElementModel::Transition *_tr_;					\
									\
  NEW(_tr_, ElementModel::Transition);					\
  _tr_->frequency = 0;							\
  _tr_->id = ((ElementModel::Node *)_from_)->successors.count() + 1;	\
  INC_TRANSITION_FREQUENCY(_from_, _tr_);				\
  _tr_->node = _to_;							\
									\
  ((ElementModel::Node *)_from_)->successors.append(_tr_);		\
}



/*!
  \brief Creates an element graph node.

  \param _pred_ Pointer to the predecessor.
  \param _node_ The created node (output variable).
  \param _type_ The type of the node.
  \param _modeler_ The modeler of the element (used only for element nodes).
*/
#define CREATE_GRAPH_NODE(_pred_, _node_, _type_, _modeler_)	\
{								\
  if (_type_ == ElementModel::StartNode)			\
    {								\
      ElementModel::AttributeNode *_tmpn_;			\
      NEW(_tmpn_, ElementModel::AttributeNode);			\
      _tmpn_->yesAttrCnt = 0;					\
      _tmpn_->noAttrCnt = 0;					\
      /* by default, elements are supposed not */		\
      /* to have attributes */					\
      _tmpn_->hasAttr = false;					\
      _node_ = _tmpn_;						\
    }								\
  else								\
    {								\
      ElementModel::Node *_tmpn_;				\
      NEW(_tmpn_, ElementModel::Node);				\
      _node_ = (ElementModel::AttributeNode *)_tmpn_;		\
    }								\
  _node_->type = _type_;					\
  _node_->modeler = _modeler_;					\
  _node_->id = nodeCounter;					\
  _node_->mpt = 0;						\
  nodeList.append(_node_);					\
  nodeCounter++;						\
  _node_->successors.setAutoDelete(true);			\
								\
 if (_pred_)							\
   {								\
     CREATE_GRAPH_TRANSITION(_pred_, _node_);			\
   }								\
}



/*!
  \brief Returns the most probable transition leading from the node.

  \param _node_ Pointer to the graph node.
  \param _mpt_ Pointer to the most probable transition (output variable).
  \param _definite_ Indication whether the most probable transition is definie (output variable).
*/
#define GET_MOST_PROBABLE_TRANSITION(_node_, _mpt_, _definite_)			\
{										\
  if (_node_->successors.isEmpty())						\
    {										\
      _mpt_ = 0;								\
      _definite_ = true;							\
    }										\
  else										\
    {										\
      _mpt_ = _node_->successors.first();					\
      _node_->successors.next();						\
      _definite_ = true;							\
      										\
      for (ElementModel::Transition *_tr_ = _node_->successors.current();	\
	   _tr_; _tr_ = _node_->successors.next())				\
	{									\
	  if (_tr_->frequency == _mpt_->frequency)				\
            {									\
	      _mpt_ = 0;							\
	      _definite_ = false;						\
            }									\
          else									\
            if (_tr_->frequency > _mpt_->frequency)				\
              {									\
	        _mpt_ = _tr_;							\
	        _definite_ = true;						\
              }									\
	}									\
    }										\
}




/*!
  \brief Finds a transition leading to the desired node.

  \param _tr_ The found transition (output variable).
  \param _node_ The pointer to the graph node.
  \param _type_ The type of the desired node.
  \param _modeler_ The modeler of the desired element (used only for nodes of type ElementNode).
*/
#define FIND_TRANSITION(_tr_, _node_, _type_, _modeler_)		\
{									\
  _tr_ = 0;								\
  for (ElementModel::Transition *_t_ = _node_->successors.first();	\
       _t_; _t_ = _node_->successors.next())				\
    {									\
      if (_t_->node->type == _type_)					\
        {								\
          if (_t_->node->type != ElementModel::ElementNode)		\
            {								\
	      _tr_ = _t_;						\
              INC_TRANSITION_FREQUENCY(_node_, _t_);			\
	      break;							\
	    }								\
	  else								\
	    if (_t_->node->modeler == _modeler_)			\
              {								\
	        _tr_ = _t_;						\
                INC_TRANSITION_FREQUENCY(_node_, _tr_);			\
	        break;							\
	      }								\
        }								\
    }									\
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/*!
  An initialization is performed.
*/
ElementModeler::ElementModeler(XmlChar *name)
{
  elementName = name;
  nodeCounter = 0;
  refCount = 1;
  structuralEntropy = 0;

  CREATE_GRAPH_NODE(0, startNode, ElementModel::StartNode, this);
  CREATE_GRAPH_NODE(0, endNode, ElementModel::EndNode, this);
  resetCurrentNode();

  //this causes the nodes and the transitions to be deleted
  //in the destructor
  nodeList.setAutoDelete(true);
}



/*!
  Does nothing.
*/
ElementModeler::~ElementModeler(void)
{
}


/*!
  Computes the structural entropy of the model.
*/
void ElementModeler::computeStructuralEntropy(void)
{
  structuralEntropy = ElementModelEntropyCalculator::calculate(this);
}


/*!
  Returns the structural entropy of the model.
  \return The structural entropy of the model.
*/
double ElementModeler::getStructuralEntropy(void)
{
  return structuralEntropy;
}


/*!
  Sets currentNode to point to startNode.
*/
void ElementModeler::resetCurrentNode(void)
{
  currentNode = startNode;
}



/*!
  Pops the node (if there is any) from the node stack and sets currentNode to it.
*/
void ElementModeler::popCurrentNode(void)
{
  if (!nodeStack.isEmpty())
    {
      currentNode = nodeStack.pop();
      DBG("POPPED: " << currentNode->type);
    }
  else
    {
      DBG("NODE STACK IS EMPTY");
    }
}


/*!
  Returns the pointer to the current node.

  \return The pointer to current node.
*/
ElementModel::Node *ElementModeler::getCurrentNode(void)
{
  return currentNode;
}



/*!
  The modeler attemps to move to the desired node from the current node. If the desired node doesn't exist, a new node is created. If the desired node exists and the most probable edge leads to it, the modeler moves along this edge. If an edge that is not the most probable leads to the desired node, the id of the edge is returned.

  \param desiredNodeType The type of the desired node.
  \param edgeId The id if the edge that is not most probable (output variable).
  \param elementPushes The number of pushes onto the node stack performed.
  \param elementModeler The pointer to the modeler of desired element (used only if desiredNode is of ElementNode type).

  \retval ElementModel::Definite Most probable edge was used.
  \retval ElementModel::Indefinite An edge that is not the most probable one was used.
  \retval ElementModel::NewNodeCreated New graph node has been created.
*/
ElementModel::TransitionState ElementModeler::moveToDesiredNode(ElementModel::NodeType desiredNodeType, size_t *edgeId, size_t *elementPushes, ElementModeler *elementModeler = 0)
{
  ElementModel::Transition *transition;	//the most probable transition from the current node
  ElementModel::Transition *mpt = currentNode->mpt;

  FIND_TRANSITION(transition, currentNode, desiredNodeType, elementModeler);

  if (!transition)
    {
      //there is no transition of desired type --> we need to create it
      ElementModel::Node *newNode;

      //create the desired successor
      if (desiredNodeType == ElementModel::ElementNode)
	{
	  //element node successor
	  //create new element node successor
	  CREATE_GRAPH_NODE(currentNode, newNode, desiredNodeType, elementModeler);


	  //remember the node that leads to new element model
	  nodeStack.push(newNode);
	}
      else
	if (desiredNodeType == ElementModel::EndNode)
	  {
	    //connect with the end node

	    newNode = endNode;

	    CREATE_GRAPH_TRANSITION(currentNode, newNode);
	  }
	else
	  {
	    //create other successor
	    // 	    DBG("ordinary node created");
	    CREATE_GRAPH_NODE(currentNode, newNode, desiredNodeType, 0);
	  }

      currentNode = newNode;

      return ElementModel::NewNodeCreated;
    }
  else
    {
      //there is  a transition of desired type
      if (transition == mpt)
	{
 	  //DBG("used mpt definite");

	  //the found transition is the most probable transition

	  ElementModel::TransitionState result = ElementModel::Definite;

	  switch (desiredNodeType)
	    {
	    case ElementModel::EndNode:
	      // 	      DBG("definite end node");
	      currentNode = transition->node;
	      *edgeId = transition->id;
	      return result;

	    case ElementModel::CharactersNode:
	      // 	      DBG("definite character node");
	      currentNode = transition->node;
	      *edgeId = transition->id;
	      return result;
		  
	    case ElementModel::ElementNode:
	      // 	      DBG("definite element node");

	      //remember the node that leads to new element model
	      nodeStack.push(transition->node);
	      *edgeId = transition->id;
	      currentNode = transition->node;
	      *elementPushes = (*elementPushes) + 1;

	      return result;
		  
	    default:
	      return ElementModel::Impossible;
	    }
	}
      else
	{
	  //the found transition either isn't the mpt
 	  //DBG("not mpt");

	  ElementModel::TransitionState result = ElementModel::Indefinite;

	  switch (desiredNodeType)
	    {
	    case ElementModel::EndNode:
	      currentNode = transition->node;
	      *edgeId = transition->id;
	      return result;

	    case ElementModel::CharactersNode:
	      currentNode = transition->node;
	      *edgeId = transition->id;
	      return result;
	      
	    case ElementModel::ElementNode:
	      //remember the node that leads to new element model
	      *elementPushes = (*elementPushes) + 1;
	      nodeStack.push(transition->node);
	      currentNode = transition->node;
	      *edgeId = transition->id;
	      return result;
	      
	    default:
	      return ElementModel::Impossible;
	    }
	}
    }

  //return ElementModel::Impossible;
}



/*!
  Move forward along the most probable edge.

  \param node Current node (output variable).
  \param saxEmitter Pointer to the sax emitter.
  \param userData Pointer to the user data structure.

  \return The type of the new current node.
*/
ElementModel::NodeType ElementModeler::moveForward(ElementModel::Node **node, SAXEmitter *saxEmitter, void *userData)
{
  if (!currentNode->mpt)
    {
      //there is no transition of desired type --> we need to create it
      return ElementModel::NoNode;
    }
  else
    {
      INC_TRANSITION_FREQUENCY(currentNode, currentNode->mpt);

      currentNode = currentNode->mpt->node;
      *node = currentNode;

      //when moving, emit SAX events reflecting the structure of the graph
      switch(currentNode->type)
	{
	case ElementModel::EndNode:
	  saxEmitter->endElement(userData, elementName);
	  break;

	case ElementModel::ElementNode:
	  //remember the node that leads to new element model
	  // 	  *elementPushes = (*elementPushes) + 1;
	  nodeStack.push(currentNode);
	  break;

	default: ;
	}
    }

  return currentNode->type;
}



/*!
  Move forward along the given edge.

  \param edgeId The id of the edge.
  \param node Current node (output variable).
  \param saxEmitter Pointer to the sax emitter.
  \param userData Pointer to the user data structure.

  \return The type of the new current node.
*/
ElementModel::NodeType ElementModeler::followEdge(size_t edgeId, ElementModel::Node **node, SAXEmitter *saxEmitter, void *userData)
{
  //important: first edge has id 1!
  ElementModel::Transition *tr;

  for (tr = currentNode->successors.first(); tr; tr = currentNode->successors.next())
    {
      if (tr->id == edgeId)
	break;
    }

  if (!tr)
    {
      //there is no transition with desired id --> report it (should never happen)
      return ElementModel::NoNode;
    }
  else
    {
      INC_TRANSITION_FREQUENCY(currentNode, tr);

      currentNode = tr->node;
      *node = currentNode;

      //when moving, emit SAX events reflecting the structure of the graph
      switch(currentNode->type)
	{
	case ElementModel::EndNode:
	  saxEmitter->endElement(userData, elementName);
	  break;

	case ElementModel::ElementNode:
	  //remember the node that leads to new element model
	  // 	  *elementPushes = (*elementPushes) + 1;
	  nodeStack.push(currentNode);
	  break;

	default: ;
	}
    }

  return currentNode->type;
}



/*!
  Returns the type of the current node in the element model.

  \return The type fo the node.
*/
ElementModel::NodeType ElementModeler::currentNodeType(void)
{
  if (!currentNode)
    return ElementModel::NoNode;
  else
    return currentNode->type;
}


/*!
  Updates the attribute prediction counts.

  \param attributes Indication if the element has attributes, or not.

  \return The supposed attribute state before the update.
*/
bool ElementModeler::setAttributes(bool attributes)
{
  bool result = hasAttributes();

  if (attributes)
    {
      startNode->yesAttrCnt++;
      if (startNode->yesAttrCnt >= startNode->noAttrCnt)
	startNode->hasAttr = true;
    }
  else
    {
      startNode->noAttrCnt++;
      if (startNode->noAttrCnt >= startNode->yesAttrCnt)
	startNode->hasAttr = false;
    }

  //return the supposed atribute state before the update
  return result;
}



/*!
  Returns the supposed attribute state of the element.

  \return The supposed attribute state of the element.
*/
bool ElementModeler::hasAttributes(void)
{
  DBG("YES: " << startNode->yesAttrCnt);
  DBG("NO: " << startNode->noAttrCnt);
  return startNode->hasAttr;
}



/*!
  Returns a pointer to the start node of the element model.

  \return The pointer to the start node.
*/
ElementModel::Node *ElementModeler::getStartNode(void)
{
  return startNode;
}



/*!
  Returns a pointer to the end node of the element model.

  \return The pointer to the end node.
*/
ElementModel::Node *ElementModeler::getEndNode(void)
{
  return endNode;
}



/*!
  Returns the name the element modeled.

  \return The name of the element.
*/
XmlChar *ElementModeler::getElementName(void)
{
  return elementName;
}


/*!
  Increases the reference count of the model.
*/
void ElementModeler::increaseRefCount(void)
{
  refCount++;
}


/*!
  Returns the actual reference count of the model.

  \return The reference count.
*/
size_t ElementModeler::getRefCount(void)
{
  return refCount;
}



/*!
  Displays a textual representation of the element graph.
*/
void ElementModeler::print(void)
{
  OUTPUTENL("  Model for element \"" << elementName << "\"");
  OUTPUTENL("    Number of references:\t\t" << refCount);
  OUTPUTENL("    Has attributes (Yes/No):\t\t" << startNode->yesAttrCnt << "/" << startNode->noAttrCnt);
  OUTPUTENL("    Structural entropy:\t\t\t" << structuralEntropy);
  OUTPUTEENDLINE;

  printNode(startNode);
  printNode(endNode);

  
  OUTPUTEENDLINE;

  //   if (!nodeStack.isEmpty())
  //     {
  //       OUTPUTENL("NODE STACK TOP: " << nodeStack.top()->modeler);
  //     }
}



/*!
  Displays a textual representation of the given element graph node.

  \param node Pointer to the graph node.
*/
void ElementModeler::printNode(ElementModel::Node *node)
{
//   if (node == currentNode)
//     {
//       OUTPUTE("-> ");
//     }
//   else
//     {
//       OUTPUTE("   ");
//     }


  OUTPUTE("      " << node->id << " (");
  switch (node->type)
    {
    case ElementModel::StartNode:
      OUTPUTE(STR_NODE_TYPE_START_NODE);
      break;
       
    case ElementModel::EndNode:
      OUTPUTE(STR_NODE_TYPE_END_NODE);
      break;
       
    case ElementModel::ElementNode:
      OUTPUTE(STR_NODE_TYPE_ELEMENT_NODE);
      break;
       
    case ElementModel::CharactersNode:
      OUTPUTE(STR_NODE_TYPE_CHARACTERS_NODE);
      break;
       
    default:;
    }

  if (node->type == ElementModel::ElementNode)
    {
      OUTPUTE(": " << node->modeler->elementName);
    }

  OUTPUTE("): ");

  bool first = true;
  for (ElementModel::Transition *ss = node->successors.first(); ss; ss = node->successors.next())
    {

      if (!first)
	{
	  OUTPUTE(", ");
	}
      else
	first = false;

      if (ss == node->mpt)
	{
	  OUTPUTE("*");
	}

      OUTPUTE(ss->node->id << "[" << ss->frequency << "]");
    }

  OUTPUTEENDLINE;

  for (ElementModel::Transition *ss = node->successors.first(); ss; ss = node->successors.next())
    {
      if (ss->node->type != ElementModel::EndNode)
	printNode(ss->node);
    }
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/*!
  This method computes the structural entropy of the model of the element. The model is accessed via the corresponding element modeler.

  \param modeler Pointer to element modeler.

  \return The structural entropy of the model.
*/
double ElementModelEntropyCalculator::calculate(ElementModeler *modeler)
{
  ElementModel::Node *startNode;
  ElementModel::Node *endNode;
  ElementModel::Transition *tr;
  size_t nodeTransitionsTotal;
  double structuralEntropy = 0;
  ElementModelTransitionInfo *tri;

  Stack<ElementModelTransitionInfo> trStack;
  trStack.setAutoDelete(true);

  startNode = modeler->getStartNode();
  endNode = modeler->getEndNode();

  nodeTransitionsTotal = 0;


  for (tr = startNode->successors.first(); tr; tr = startNode->successors.next())
    {
      nodeTransitionsTotal += tr->frequency;
    }

  for (tr = startNode->successors.first(); tr; tr = startNode->successors.next())
    {
      NEW(tri, ElementModelTransitionInfo);
      tri->transition = tr;
      tri->probability = (double)tr->frequency / nodeTransitionsTotal;

      trStack.push(tri);
    }



  while (!trStack.isEmpty())
    {
      tri = trStack.pop();
      ElementModel::Transition *t = tri->transition;
      double prob = tri->probability;


      if (t->node == endNode)
	{
	  //reached the end node
	  structuralEntropy += tri->probability*abs(log(tri->probability));
	}
      else
	{
	  nodeTransitionsTotal = 0;

	  for (tr = t->node->successors.first(); tr; tr = t->node->successors.next())
	    {
	      nodeTransitionsTotal += tr->frequency;
	    }
	  
	  for (tr = t->node->successors.first(); tr; tr = t->node->successors.next())
	    {
	      NEW(tri, ElementModelTransitionInfo);
	      tri->transition = tr;
	      tri->probability = (prob * tr->frequency) / nodeTransitionsTotal;
	      trStack.push(tri);
	    }
	}
    }

  trStack.clear();

  return structuralEntropy;
}
