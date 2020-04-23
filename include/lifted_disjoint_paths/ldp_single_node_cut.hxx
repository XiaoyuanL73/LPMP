#pragma once
#include <andres/graph/digraph.hxx>
#include <unordered_map>
#include <unordered_set>
#include <stack>


namespace LPMP {

struct baseAndLiftedMessages{
		std::unordered_map<size_t,double> baseMessages;
		std::unordered_map<size_t,double> liftedMessages;
	};

template<class LDP_STRUCT>
class ldp_single_node_cut_factor
{
public:
	//constexpr static std::size_t no_edge_active = std::numeric_limits<std::size_t>::infinity();

	//template<class LPD_STRUCT> ldp_single_node_cut_factor(const LPD_STRUCT& ldpStruct);
	ldp_single_node_cut_factor(const LDP_STRUCT& ldpStruct,size_t nID,bool isOut):
		baseGraph(ldpStruct.getGraph()),
		//liftedGraph(ldpStruct.getGraphLifted()),
		nodeID(nID),
		ldpStructure(ldpStruct),
		isOutFlow(isOut),
		nodeNotActive(nID)
	{


		primal_=nodeNotActive;  //corresponds to no edge active
		optimalSolution=nodeNotActive;

		if(isOutFlow){
			minLayer=ldpStruct.getGroupIndex(nodeID);
			maxLayer=minLayer+ldpStruct.getGapLifted(); //some method that returns max time gap lifted
		}
		else{
			maxLayer=ldpStruct.getGroupIndex(nodeID);
			minLayer=std::max(0,int(maxLayer)-int(ldpStruct.getGapLifted()));
		}

		baseCosts=std::unordered_map<size_t,double>();  //TODO change to maps nodeID->cost. Should be easier for messages
		liftedCosts=std::unordered_map<size_t,double>();
		solutionCosts=std::unordered_map<size_t,double>();
		solutionCosts[nodeNotActive]=0;
		vsUpToDate=false;

	}


	double LowerBound() const;
	double EvaluatePrimal() const{
		double value=solutionCosts.at(primal_);
		return value;
	}
	std::unordered_map<size_t,double> adjustCostsAndSendMessages();



	baseAndLiftedMessages adjustCostsAndSendMessagesLifted();


	template<class ARCHIVE> void serialize_primal(ARCHIVE& ar) { ar(primal_); }
	template<class ARCHIVE> void serialize_dual(ARCHIVE& ar) { ar(); }

	//auto export_variables() { return std::tie(*static_cast<std::size_t>(this)); }//TODO change this. This will not work with so many variables
	auto export_variables() { return std::tie(solutionCosts); }//?What comes here?

	void init_primal() { primal_ = nodeNotActive; }

	void updateCostSimple(const double value,const size_t vertexIndex,bool isLifted);

	void updateValues() const;

	const andres::graph::Digraph<>& getBaseGraph() const {
		return baseGraph;
	}

	const std::size_t nodeID;
	const size_t nodeNotActive;


private:

	size_t getNeighborBaseVertex(size_t firstNode,size_t neighborIndex) const{
		if(isOutFlow){
			return baseGraph.vertexFromVertex(firstNode,neighborIndex);
		}
		else{
			return baseGraph.vertexToVertex(firstNode,neighborIndex);
		}
	}
	size_t numberOfNeighborsBase(size_t nodeIndex) const {
		if(isOutFlow){
			return baseGraph.numberOfEdgesFromVertex(nodeIndex);
		}
		else{
			return baseGraph.numberOfEdgesToVertex(nodeIndex);
		}
	}
	bool isInRange(size_t nodeIndex) const {
		if(isOutFlow){
			return ldpStructure.getGroupIndex(nodeIndex)<=maxLayer;
		}
		else{
			return ldpStructure.getGroupIndex(nodeIndex)>=minLayer;
		}
	}



	std::size_t primal_; // the incoming resp. outgoing edge that is active.
	mutable std::size_t optimalSolution;

	std::size_t minLayer;
	std::size_t maxLayer;

	const bool isOutFlow;

	const andres::graph::Digraph<>& baseGraph;
	//const andres::graph::Digraph<>& liftedGraph;
	const LDP_STRUCT& ldpStructure;


	mutable std::unordered_map<size_t,double> baseCosts;
	std::unordered_map<size_t,double> liftedCosts;
	mutable std::unordered_map<size_t,double> solutionCosts;
	mutable std::unordered_map<size_t,std::unordered_set<size_t>> indexStructure;

	mutable std::unordered_map<size_t,double> valuesStructure;  //For DFS procedure

	mutable bool vsUpToDate;

	//	 size_t getNeighborBaseEdge(size_t firstNode,size_t neighborIndex){
	//		 if(isOutFlow){
	//			 return baseGraph.edgeFromVertex(firstNode,neighborIndex);
	//		 }
	//		 else{
	//			 return baseGraph.edgeToVertex(firstNode,neighborIndex);
	//		 }
	//	 }
	//	 size_t getNeighborLiftedEdge(size_t firstNode,size_t neighborIndex){
	//		 if(isOutFlow){
	//			 return liftedGraph.edgeFromVertex(firstNode,neighborIndex);
	//		 }
	//		 else{
	//			 return liftedGraph.edgeToVertex(firstNode,neighborIndex);
	//		 }
	//	 }
	//	 size_t getNeighborLiftedVertex(size_t firstNode,size_t neighborIndex){
	//		 if(isOutFlow){
	//			 return liftedGraph.vertexFromVertex(firstNode,neighborIndex);
	//		 }
	//		 else{
	//			 return liftedGraph.vertexToVertex(firstNode,neighborIndex);
	//		 }
	//	 }

	//	 size_t numberOfNeighborsLifted(size_t nodeIndex){
	//		 if(isOutFlow){
	//			 return liftedGraph.numberOfEdgesFromVertex(nodeIndex);
	//		 }
	//		 else{
	//			 return liftedGraph.numberOfEdgesToVertex(nodeIndex);
	//		 }
	//	 }

	//	 bool reachable(size_t firstVertex,size_t secondVertex){
	//		 if(isOutFlow){
	//			 return ldpStructure.isReachable(firstVertex,secondVertex);
	//		 }
	//		 else{
	//			 return ldpStructure.isReachable(secondVertex,firstVertex);
	//		 }
	//	 }


	//	 std::pair<bool,size_t> findEdgeBase(size_t firstNode,size_t secondNode){
	//		 if(isOutFlow){
	//			 return baseGraph.findEdge(firstNode,secondNode);
	//		 }
	//		 else{
	//			 return baseGraph.findEdge(secondNode,firstNode);
	//		 }
	//
	//	 }
	//	 std::pair<bool,size_t> findEdgeLifted(size_t firstNode,size_t secondNode){
	//		 if(isOutFlow){
	//			 return liftedGraph.findEdge(firstNode,secondNode);
	//		 }
	//		 else{
	//			 return liftedGraph.findEdge(secondNode,firstNode);
	//		 }
	//	 }


	//	size_t getVertexToReach(){
	//		if(isOutFlow){
	//			return ldpStructure.getTerminalNode;
	//		}
	//		else{
	//			return ldpStructure.getSourceNode;
	//		}
	//	}

};



template<class LDP_STRUCT>
inline baseAndLiftedMessages ldp_single_node_cut_factor<LDP_STRUCT>::adjustCostsAndSendMessagesLifted(){
	std::unordered_map<size_t,double> baseMessages=adjustCostsAndSendMessages();
	std::unordered_set<size_t> isNotZeroInOpt;
	std::unordered_set<size_t> isOneInOpt;
	//TODO try other starting points then optimalSolution
	std::stack<size_t> myStack;
	myStack.push(optimalSolution);
	std::vector<size_t> path;
	std::unordered_set<size_t> isOnPath;
	std::unordered_map<size_t,size_t> predOnPath;
	bool pathClosed=false;


	size_t currentVertex=optimalSolution;
	while(!pathClosed){
		path.push_back(currentVertex);
		isOnPath.insert(currentVertex);
		isOneInOpt.insert(currentVertex);
		isNotZeroInOpt.insert(currentVertex);
		if(indexStructure.count(currentVertex)>0){
			std::unordered_set<size_t>& myDesc=indexStructure[currentVertex];
			if(myDesc.size()>1){
				auto it=myDesc.begin();
				it++;
				for(;it!=myDesc.end();it++){
					size_t sibling=*it;
					myStack.push(sibling);
					predOnPath[sibling]=currentVertex;
				}
			}
			currentVertex=(*myDesc.begin());
		}
		else{
			pathClosed=true;
		}
	}


	size_t currentPathStart;  //does it need to be init?

	while(!myStack.empty()){
		size_t currentVertex=myStack.top();
		myStack.pop();
		if(predOnPath.count(currentVertex)>0){
			currentPathStart=predOnPath[currentVertex];
		}
		bool hasDesc=indexStructure.count(currentVertex)>0;
		if(isOnPath.count(currentVertex)>0||!hasDesc){
			if(!hasDesc){
				isOneInOpt.insert(currentVertex);
			}
			//TODO traverse path
		}
		else{
			isOneInOpt.insert(currentVertex);
			std::unordered_set<size_t>& myDesc=indexStructure[currentVertex];
			for(auto it=myDesc.begin();it!=myDesc.end();it++){
				size_t vertex=*it;
				myStack.push(vertex);
			}
		}
	}


}





template<class LDP_STRUCT>
inline std::unordered_map<size_t,double> ldp_single_node_cut_factor<LDP_STRUCT>::adjustCostsAndSendMessages(){
	if(!vsUpToDate) updateValues();
	double minValue=solutionCosts[optimalSolution];
	std::unordered_map<size_t,double> messages;
	for (auto it=solutionCosts.begin();it!=solutionCosts.end();it++) {
		if(it->first==optimalSolution) continue;
		double delta=it->second-minValue;
		messages[it->first]=delta;
		baseCosts[it->first]-=delta;
		it->second=minValue;
	}
	return messages;
}


template<class LDP_STRUCT>
inline void ldp_single_node_cut_factor<LDP_STRUCT>::updateCostSimple(const double value,const size_t vertexIndex,bool isLifted){//Only cost change
	if(!isLifted){ //update in base edge
		baseCosts[vertexIndex]+=value;
		solutionCosts[vertexIndex]+=value;
	}
	else{ //update in lifted edge
		liftedCosts[vertexIndex]+=value;
		valuesStructure[vertexIndex]+=value;
		vsUpToDate=false;
	}
}


template<class LDP_STRUCT>
inline double ldp_single_node_cut_factor<LDP_STRUCT>::LowerBound() const{//TODO store info about how valuesStructures changed. At least max time layer of changed lifted edge
	if(!vsUpToDate){
		updateValues();
		return solutionCosts.at(optimalSolution);
	}
	else{
		double minValue=solutionCosts.at(optimalSolution);
		size_t minVertex=optimalSolution;
		for (std::pair<size_t,double> it :solutionCosts) {
			if(it.second<minValue){
				minValue=it.second;
				minVertex=it.first;
			}
		}
		optimalSolution=minVertex;
		return solutionCosts.at(optimalSolution);
	}
}


template<class LDP_STRUCT>
inline void ldp_single_node_cut_factor<LDP_STRUCT>::updateValues() const{
	//TODO check if everything works fine for edge from v to t
	std::unordered_set<size_t> closedVertices;
	indexStructure=std::unordered_map<size_t,std::unordered_set<size_t>>();
	valuesStructure=std::unordered_map<size_t,double>();
	//size_t vertexToReach=getVertexToReach();

	std::stack<size_t> nodeStack;
	nodeStack.push(nodeID);

	while(!nodeStack.empty()){
		size_t currentNode=nodeStack.top();
		bool descClosed=true;
		double minValue=0;
		std::unordered_set<size_t> minValueIndices;
		//size_t minValueIndex=vertexToReach;

		for (int i = 0; i < numberOfNeighborsBase(currentNode); ++i) {
			//size_t desc=baseGraph.vertexFromVertex(currentNode,i);
			size_t desc=getNeighborBaseVertex(currentNode,i);
			if(isInRange(desc)){
				//if(indexStructure.count(desc)>0||(oneEdgeUpdate&&!reachable(currentNode,vertexToReach))){  //descendant closed
			    if(closedVertices.count(desc)>0){  //descendant closed
					if(descClosed&&valuesStructure.count(desc)>0){
						if(minValue>=valuesStructure[desc]&&valuesStructure[desc]<0){
							minValue=valuesStructure[desc];
						}
					}
				}
				else{  //descendant not closed
					nodeStack.push(desc);
					descClosed=false;

				}
			}
		}
		if(descClosed){ //Close node if all descendants are closed
			if(currentNode==nodeID){  //all nodes closed, compute solution values
				double bestValue=0;
				size_t bestVertex=nodeNotActive;
				for (auto it=baseCosts.begin();it!=baseCosts.end();it++) {
					size_t vertex=it->first;
					double baseCost=it->second;
					double valueToAdd=0;
					if(valuesStructure.count(vertex)>0){
						valueToAdd=valuesStructure[vertex];
					}
					else{
						if(liftedCosts.count(vertex)>0){
							valueToAdd=liftedCosts.at(vertex);
						}
					}
					solutionCosts[vertex]=baseCost+valueToAdd;
					if(solutionCosts[vertex]<bestValue){
						bestValue=solutionCosts[vertex];
						bestVertex=vertex;
					}
				}
				optimalSolution=bestVertex;

			}
			else{
				if(minValue<0){
					for (int i = 0; i < numberOfNeighborsBase(currentNode); ++i) {
						size_t desc=getNeighborBaseVertex(currentNode,i);
						if(isInRange(desc)){
							if(valuesStructure.count(desc)>0&&valuesStructure[desc]==minValue){
								indexStructure[currentNode].insert(desc);
							}
						}
					}
				}
				if(liftedCosts.count(currentNode)>0){
					minValue+=liftedCosts.at(currentNode);  //add lifted edge cost if the edge exists
				}
				if(minValue<0||(baseCosts.count(currentNode)>0&&minValue<liftedCosts.at(currentNode))){  //store only negative values or values needed to correct solutionCosts
					valuesStructure[currentNode]=minValue;
				}
				else{
					valuesStructure.erase(currentNode);
				}
				closedVertices.insert(currentNode); //marking the node as closed.
			}
			nodeStack.pop();
		}
	}

	vsUpToDate=true;

}



//template<class LDP_STRUCT>
//inline void ldp_single_node_cut_factor<LDP_STRUCT>::updateCostFull(const double value,const size_t index){//Includes DFS update
//	size_t myIndex=decodeIndex(index);
//	if(myIndex<numberOfEdges){ //update in base edge
//		baseCosts[myIndex]+=value;
//		solutionCosts[myIndex]+=value;
//		if(myIndex==optimalSolution){
//			if(value>0){
//				for (int i = 0; i < numberOfEdges; ++i) {
//					if(solutionCosts[i]<solutionCosts[optimalSolution]){
//						optimalSolution=i;
//						primal_=i;
//					}
//				}
//			}
//		}
//		else if(solutionCosts[myIndex]<solutionCosts[optimalSolution]){
//			optimalSolution=myIndex;
//			primal_=myIndex; //maybe not
//
//		}
//
//	}
//	else{ //update in lifted edge
//		liftedCosts[myIndex-numberOfEdges]+=value;
//		updateValues(myIndex-numberOfEdges);
//	}
//}





class ldp_mcf_single_node_cut_message
{
	template<typename SINGLE_NODE_CUT_FACTOR>
	void RepamLeft(SINGLE_NODE_CUT_FACTOR& r, const double msg, const std::size_t msg_dim) const
	{
		r.updateCostSimple(msg,msg_dim);
		//Only base edges are updated if message comes from mcf, the dfs procedure not needed

	}

	template<typename MCF_FACTOR>
	void RepamRight(MCF_FACTOR& r, const double msg, const std::size_t msg_dim) const
	{
	}

	template<typename SINGLE_NODE_CUT_FACTOR, typename MSG>
	void send_message_to_left(const SINGLE_NODE_CUT_FACTOR& r, MSG& msg, const double omega = 1.0)
	{
	}

	template<typename MCF_FACTOR, typename MSG_ARRAY>
	static void SendMessagesToRight(const MCF_FACTOR& leftRepam, MSG_ARRAY msg_begin, MSG_ARRAY msg_end, const double omega)
	{
	}
};

}
