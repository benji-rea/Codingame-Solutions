#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <cassert>

using namespace std;

// for optimization
// 1. remove disconnected gateway nodes from nodes and gateways

static const int kMaxDistance = 1000;

class Node
{
public:
	Node() : _isGateway( false ), _distance( 0 ) {}

	size_t GetNumberOfLinkedNodes() const { return _linkedNodeIDs.size(); }
	int GetLinkedNodeID( int index ) const { return _linkedNodeIDs[ index ]; }

	void SetIsGateway( bool isGateway ) { _isGateway = isGateway; }
	bool IsGateway() const { return _isGateway; }

	void AddLinkedNode( int to );
	void RemoveLinkedNode( int ID );
	bool IsLinkedNode( int ID ) const;

	void SetDistanceFromSkynetAgent( int distance ) { _distance = distance; }
	int GetDistanceFromSkynetAgent() const { return _distance; }

	int GetNumberOfAdjacentGateways( const vector<Node> &nodes ) const;
private:
	bool _isGateway;
	vector<int> _linkedNodeIDs;
	int _distance;
};

static void SeverLink( vector<Node> &nodes, vector<int> &gateways, int SkynetAgentPos );

int main()
{
	int N; // the total number of nodes in the level, including the gateways
	int L; // the number of links
	int E; // the number of exit gateways
	cin >> N >> L >> E; cin.ignore();

	vector<Node> nodes( N );
	vector<int> gateways( E );

	for( int i = 0; i < L; i++ ) {
		int N1; // N1 and N2 defines a link between these nodes
		int N2;
		cin >> N1 >> N2; cin.ignore();

		nodes[ N1 ].AddLinkedNode( N2 );
		nodes[ N2 ].AddLinkedNode( N1 );
	}

	for( int i = 0; i < E; i++ ) {
		int EI; // the index of a gateway node
		cin >> EI; cin.ignore();

		//gateways.push_back( EI );
		gateways[ i ] = EI;
		nodes[ EI ].SetIsGateway( true );
	}

	int nodeID = 0;
	for( auto node : nodes )
	{
		cerr << nodeID << " : ";
		for( size_t i = 0; i < node.GetNumberOfLinkedNodes(); ++i )
		{
			if( i > 0 )
			{
				cerr << ", ";
			}
			cerr << node.GetLinkedNodeID( i );
			if( node.IsGateway() )
			{
				cerr << "g";
			}
		}
		cerr << endl;

		++nodeID;
	}

	// game loop
	while( 1 ) {
		int SI; // The index of the node on which the Skynet agent is positioned this turn
		cin >> SI; cin.ignore();

		SeverLink( nodes, gateways, SI );
	}
}


// class Node implementation
void Node::AddLinkedNode( int to )
{
	if( find( _linkedNodeIDs.begin(), _linkedNodeIDs.end(), to ) == _linkedNodeIDs.end() )
	{
		_linkedNodeIDs.push_back( to );
		sort( _linkedNodeIDs.begin(), _linkedNodeIDs.end() );
	}
}

void Node::RemoveLinkedNode( int ID )
{
	auto it = find( _linkedNodeIDs.begin(), _linkedNodeIDs.end(), ID );
	assert( it != _linkedNodeIDs.end() );

	_linkedNodeIDs.erase( it );
}

bool Node::IsLinkedNode( int ID ) const
{
	auto it = find( _linkedNodeIDs.begin(), _linkedNodeIDs.end(), ID );
	return ( it != _linkedNodeIDs.end() );
}

int Node::GetNumberOfAdjacentGateways( const vector<Node> &nodes )	const
{
	int numOfAdjacentGateway = 0;
	for( size_t i = 0; i < GetNumberOfLinkedNodes(); ++i )
	{
		const int adjID = GetLinkedNodeID( i );
		const Node& adjNode = nodes[ adjID ];

		if( adjNode.IsGateway() )
		{
			++numOfAdjacentGateway;
		}
	}

	return numOfAdjacentGateway;
}

// logic
static int BFSDistance( int startID, int endID, const vector<Node> &nodes );
static void Sever( int aID, int bID, vector<Node> &nodes );

static void SeverLink( vector<Node> &nodes, vector<int> &gateways, int SkynetAgentPos )
{
	// calculate all the distances from SkynetAgent to Nodes
	for( size_t i = 0; i < nodes.size(); ++i )
	{
		const int nodeID = i;
		Node& node = nodes[ nodeID ];

		const int distance = BFSDistance( SkynetAgentPos, nodeID, nodes );
		//cerr << "distance from " << SkynetAgentPos << " to " << nodeID << " : " << distance << endl;
		if( distance <= 1 && node.IsGateway() )
		{
			cerr << "sever skynet agent to gateway" << endl;
			Sever( SkynetAgentPos, nodeID, nodes );
			return;
		}

		node.SetDistanceFromSkynetAgent( distance );
	}
	cerr << "distance calculation done" << endl;

	// find maxNumOfAdjacentGateways
	int maxNumOfAdjacentGateways = 0;
	vector<int> candidatedNodeIDs;
	for( size_t i = 0; i < gateways.size(); i++ )
	{
		const int gID = gateways[ i ];
		const Node& gNode = nodes[ gID ];

		for( size_t j = 0; j < gNode.GetNumberOfLinkedNodes(); ++j )
		{
			const int lID = gNode.GetLinkedNodeID( j );
			const Node& lNode = nodes[ lID ];

			const int numOfAdjacentGateway = lNode.GetNumberOfAdjacentGateways( nodes );
			if( numOfAdjacentGateway >= maxNumOfAdjacentGateways )
			{
				if( numOfAdjacentGateway > maxNumOfAdjacentGateways )
				{
					maxNumOfAdjacentGateways = numOfAdjacentGateway;
					candidatedNodeIDs.clear();
				}
				candidatedNodeIDs.push_back( lID );
			}
		}
	}
	cerr << "maxNumOfAdjacentGateways calculation done. maxNumOfAdjacentGateways " << maxNumOfAdjacentGateways << ", candidatedNodeIDs count " << candidatedNodeIDs.size() << endl;

	int nominatedNodeId = -1;
	int maxNumberAdjacentNodeNextToGateway = 0;
	int minDistance = kMaxDistance;
	for( size_t i = 0; i < candidatedNodeIDs.size(); ++i )
	{
		const int nodeID = candidatedNodeIDs[ i ];
		const Node& node = nodes[ nodeID ];
		const int distance = node.GetDistanceFromSkynetAgent();

		// Hack
		if( node.IsLinkedNode( SkynetAgentPos ) )
		{
			nominatedNodeId = nodeID;
			break;
		}

		int numberAdjacentNodeNextToGateway = 0;
		for( size_t j = 0; j < node.GetNumberOfLinkedNodes(); ++j )
		{
			const int lID = node.GetLinkedNodeID( j );
			const Node& lNode = nodes[ lID ];

			if( lNode.GetNumberOfAdjacentGateways( nodes ) > 0 )
			{
				++numberAdjacentNodeNextToGateway;
			}
		}

		if( nominatedNodeId < 0 ||
			numberAdjacentNodeNextToGateway > maxNumberAdjacentNodeNextToGateway ||
			( ( numberAdjacentNodeNextToGateway == maxNumberAdjacentNodeNextToGateway ) && ( distance < minDistance ) ) )
		{
			nominatedNodeId = nodeID;
			maxNumberAdjacentNodeNextToGateway = numberAdjacentNodeNextToGateway;
			minDistance = distance;
		}
	}


	cerr << "nominatedNodeId " << nominatedNodeId << ", GetNumberOfLinkedGateways " << maxNumOfAdjacentGateways << endl;

	if( nominatedNodeId >= 0 )
	{
		const Node& cNode = nodes[ nominatedNodeId ];

		int minDistance = kMaxDistance;
		int nominatedGatewayID = -1;
		for( size_t j = 0; j < cNode.GetNumberOfLinkedNodes(); ++j )
		{
			const int lID = cNode.GetLinkedNodeID( j );
			const Node& lNode = nodes[ lID ];
			if( lNode.IsGateway() &&
				lNode.GetDistanceFromSkynetAgent() < minDistance )
			{
				nominatedGatewayID = lID;
				minDistance = lNode.GetDistanceFromSkynetAgent();
			}
		}

		if( nominatedGatewayID >= 0 )
		{
		    // for achievement
		    if( nodes[nominatedNodeId].IsLinkedNode( SkynetAgentPos ) )
		    {
		        cerr << "sever node to agent" << endl;
    			Sever( nominatedNodeId, SkynetAgentPos, nodes );
    			return;
		    }
		    
		    
			cerr << "sever node to gateway" << endl;
			Sever( nominatedNodeId, nominatedGatewayID, nodes );
			return;
		}
	}
}

static int BFSDistance( int startID, int endID, const vector<Node> &nodes )
{
	//cerr << "BFSDistance from " << startID << " to " << endID << endl;

	vector< int > queue;
	vector< bool > visited( nodes.size() );
	queue.push_back( startID );

	int distance = 0;
	while( queue.size() > 0 )
	{
		size_t oldQueueSize = queue.size();

		for( size_t i = 0; i < oldQueueSize; ++i )
		{
			const int nodeID = queue[ i ];

			if( nodeID == endID )
			{
				return distance;
			}

			const Node &node = nodes[ nodeID ];
			visited[ nodeID ] = true;

			int numOfLinkedNode = node.GetNumberOfLinkedNodes();
			for( int j = 0; j < numOfLinkedNode; ++j )
			{
				int linkedNodeID = node.GetLinkedNodeID( j );
				if( visited[ linkedNodeID ] == false )
				{
					queue.push_back( linkedNodeID );
				}
			}
		}

		queue.erase( queue.begin(), queue.begin() + oldQueueSize );
		++distance;
	}

	return kMaxDistance;
}

static void Sever( int aID, int bID, vector<Node> &nodes )
{
	cerr << "Sever " << aID << " " << bID << endl;
	assert( nodes[ aID ].IsLinkedNode( bID ) );

	cout << aID << " " << bID << endl;

	nodes[ aID ].RemoveLinkedNode( bID );
	nodes[ bID ].RemoveLinkedNode( aID );
}
