// 10724128 �d�t�� 10727104 ���l�� 

# include<fstream>
# include<string.h>
# include<iostream>
# include<vector>
# include<cstdlib>
# include<iomanip>
# include<stack>
 
# define MAX_DATA 2
# define MAX_CHILD 3

using namespace std;

typedef struct cT { 
	string school;   
	string major;   
	string type;
	string level;
	int index; 
	int student;	
} collegeType;

class Two_Three_Tree {
private:
	
	typedef struct slotT {  
		vector <int> rSet; // �����ۦP��Ȫ���� 
		string key; // ���
	}slotType;
	
	typedef struct nT {
		slotType data[MAX_DATA]; // ���I���
		nT *child[MAX_CHILD]; // �C�@�Ӹ`�I�̦h�T�Ӥl�`�I 
		nT *parent; // ���V���`�I 
	}nodeType;
	
	typedef struct pointT {  
		nodeType *pnode; // �j�M���|�W���`�I
		int pidx; // ���|�`�I��V 
	}pointType;
	
	typedef struct bT {
		slotType slot; 
		nodeType *link; // ���V�k�`�I 
	}blockType;
	
	fstream file; // �ɮ׳B�z
	nodeType *root; // ���Vroot  
	vector <collegeType> input; // �ʺA�}�C
	int height; // ��
	int nodeNum; // �`�I�� 
	
public:
	Two_Three_Tree() { // constructor
		root = NULL;
		height = 0;
	}

	bool Read( string num ) { // Ū�ɮ� 
		cout << endl;
		string filename,line,cut;
		filename = "input"+ num + ".txt";
		file.open( filename.c_str(), ios::in );
		if ( !file ) {
			cout << "�L���w���ɮצW��: " << filename << endl << endl;
			return false;
		}
		else {
			/* ��e�T��Ū�� */ 
			getline(file,line);
			getline(file,line);
			getline(file,line);
			/* ��e�T��Ū�� */
			collegeType buffer;
			int i = 1; // �s�� 
			while ( !file.eof() && getline(file,line) ) {
				int fNo = 0,pre = 0, pos =0;
				do {
					pos = line.find_first_of('\t',pre);
					cut = line.substr(pre,pos-pre);
					switch( ++fNo ) {
						case 2: buffer.school = cut;
							break;
						case 4: buffer.major = cut;
							break;
						case 5: buffer.type = cut;
							break;
						case 6: buffer.level = cut;
							break;
						case 7: buffer.student = atoi(cut.c_str());
							break;
						default: break;
					} // end of switch
					
					pre = ++pos;
				} while ( pos > 0 && fNo < 10 );
				
				buffer.index = i;
				input.push_back(buffer);
				i++;
			}	
		}	
			file.close();
			return true;
	}
	
	void Free23Tree( nodeType *root ) { // �R���`�I 
		if ( root != NULL ) {
			Free23Tree( root->child[0]);
			for ( int i = 0 ; i < MAX_DATA ; i++ ) {
				if ( !root->data[i].rSet.size())
					break;
				Free23Tree( root->child[i+1] ); 
			}
			delete root;
		}
	}

	void Clear() { // �M��vector 
		input.clear();
		Free23Tree(root);
		root = NULL;
	}
	
	
	int GetHeight( nodeType *walk ) { // ���o�� 
		if ( walk == NULL ) 
			return +0;
		int num[3];
		num[0] = GetHeight( walk->child[0] );
		num[1] = GetHeight( walk->child[1] );
		num[2] = GetHeight( walk->child[2] );
		
		for ( int i = 0 ; i <= 2 ; i++ ) {
			if ( num[i] > num[i+1] && i < 2 ) {
				int temp = num[i+1];
				num[i+1] = num[i];
				num[i] = temp;
			}
			
			else if ( i == 2 ) {
				break;
			}
		}
		
		return num[2]+1;
		
	}
	
	// �s�W�`�I // 
	nodeType* CreateNode( nodeType *left, nodeType *right, nodeType *pNode, slotType & newslot ) {
		nodeType *newNode = NULL;
		try {
			newNode = new nodeType();
			newNode->data[0].rSet = newslot.rSet;
			newNode->data[1].rSet.clear();
			newNode->data[0].key = newslot.key;
			newNode->data[1].key = "";
			newNode->parent = pNode;
			newNode->child[0] = left;
			newNode->child[1] = right;
			newNode->child[2] = NULL;
		}
		
		catch ( std::bad_alloc& ba ) {
			std::cerr << endl << "bad_alloc caught: " << ba.what() << endl;
		}
		
		return newNode;
	}
	// �s�W�`�I //
	
	// �s�W�s����� // 
	nodeType* CreateRoot( nodeType *pNode, nodeType *link, slotType & slot ) {
		nodeType *newRoot = CreateNode( pNode, link, NULL, slot );
		pNode->parent = newRoot;
		link->parent = newRoot;
		return newRoot;	
	}
	// �s�W�s����� //
	 
	void SplitNonLeaf( blockType & blockUp, pointType & curP ) { // ���������`�I 
		slotType buf[MAX_CHILD];
		nodeType *ptr[MAX_CHILD+1];  
		nodeType *rightNode; // �s���k�l�� 
		int idx = 0;
		for ( int i = 0 ; i < MAX_CHILD ; i++ ) { // �Ƨ� 
			buf[i].rSet = ( i == curP.pidx ) ? blockUp.slot.rSet : curP.pnode->data[idx].rSet;
			buf[i].key = ( i == curP.pidx ) ? blockUp.slot.key : curP.pnode->data[idx++].key;
		}
	
		curP.pnode->data[0].rSet = buf[0].rSet;
		curP.pnode->data[0].key = buf[0].key;
		for ( int i = 1 ; i < MAX_DATA ; i++ ) { // �d�U�̤p����� 
			curP.pnode->data[i].rSet.clear();
		}
				
		for ( int i = 0 ; i < MAX_CHILD ; i++ ) { // ����e�`�I���Ҧ��l�`�I 
			ptr[i] = curP.pnode->child[i];
		}
		
		// �Ҧ��l�`�I(�q�̤j��)�}�l�MblockUp.link��j�p //  
		for ( int i = MAX_CHILD - 1 ; i >= 0 ; i-- ) { 
			if ( blockUp.link->data[0].key.compare( ptr[i]->data[0].key ) > 0 ) {
				ptr[3] = blockUp.link;
				if ( i == MAX_CHILD - 1 ) { // blockUp.link�O�̤j�����b�s���k�l�𪺥k�`�I 
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[1];
					curP.pnode->child[2] = NULL;
					rightNode = CreateNode( ptr[2], ptr[3], 
											curP.pnode->parent, buf[2]);
					break;
				}
				
				else if ( i == MAX_CHILD - 2 ) { // blockUp.link�ĤG�j���b�s���k�l�𪺥��`�I
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[1];
					curP.pnode->child[2] = NULL;
					rightNode = CreateNode( ptr[3], ptr[2], 
											curP.pnode->parent, buf[2]);
					break;					
				}
				
				else if ( i == MAX_CHILD - 3 ) { // blockUp.link�ĤT�j���b��e�`�I���k�`�I 
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[3];
					curP.pnode->child[2] = NULL;
					rightNode = CreateNode( ptr[1], ptr[2], 
											curP.pnode->parent, buf[2]);
					break;						
				}
			}
		}
		
		// �Ҧ��l�`�I(�q�̤j��)�}�l�MblockUp.link��j�p //
	
		// ��sblockUp //  
		blockUp.slot.rSet = buf[1].rSet;  
		blockUp.slot.key = buf[1].key;
		blockUp.link = rightNode;
		// ��sblockUp //
	}
	
	void InsertNonLeaf( const blockType & blockUp, pointType & curP ) { // �s�W�����`�I
		nodeType *ptr[MAX_CHILD];
		for ( int i = MAX_DATA-1 ; i >= curP.pidx ; i-- ) {
			if ( i > curP.pidx ) { // �`�I���¸�Ʃ��k�䲾
				curP.pnode->data[i].rSet = curP.pnode->data[i-1].rSet;
				curP.pnode->data[i].key = curP.pnode->data[i-1].key;
			}		
			
			else if ( i == curP.pidx ) {  // �x�s�s���
				curP.pnode->data[i].rSet = blockUp.slot.rSet;
				curP.pnode->data[i].key = blockUp.slot.key;
			}
			
			else {
				break;
			}
		}	
		
		for ( int i = 0 ; i < MAX_CHILD - 1 ; i++ ) {  // ����e�`�I���Ҧ��l�`�I
			ptr[i] = curP.pnode->child[i];
		}
		
		// �Ҧ��l�`�I(�q�̤j��)�}�l�MblockUp.link��j�p //
		for ( int i = MAX_CHILD - 2 ; i >= 0 ; i-- ) {
			if ( blockUp.link->data[0].key.compare( ptr[i]->data[0].key ) > 0 ) {
				ptr[3] = blockUp.link;
				if ( i == MAX_CHILD - 2 ) { // blockUp.link�O�̤j�����b��e�`�I���k�`�I
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[1];
					curP.pnode->child[2] = ptr[3];
					break;
				}
				
				else if ( i == MAX_CHILD - 3 ) { // blockUp.link�O�ĤG�j�����b��e�`�I�������`�I 
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[3];
					curP.pnode->child[2] = ptr[1];
					break;					
				}
				
			}
		}
		// �Ҧ��l�`�I(�q�̤j��)�}�l�MblockUp.link��j�p //		
	}
	
	void SplitLeaf( slotType & newslot, pointType & curP, blockType & blockUp ) { // �������`�I 
		slotType buf[MAX_CHILD];
		int idx = 0; 
		for ( int i = 0 ; i < MAX_CHILD ; i++ ) { // �Ƨ� 
			buf[i].rSet = ( i == curP.pidx ) ? newslot.rSet : curP.pnode->data[idx].rSet;
			buf[i].key = ( i == curP.pidx ) ? newslot.key : curP.pnode->data[idx++].key;
		} 
		
		curP.pnode->data[0].rSet = buf[0].rSet;
		curP.pnode->data[0].key = buf[0].key;
		for ( int i = 1 ; i < MAX_DATA ; i++ ) { // �d�U�̤p����� 
			curP.pnode->data[i].rSet.clear();
		}		
		
		blockUp.link = CreateNode( NULL, NULL, curP.pnode->parent, buf[2]); 
		blockUp.slot.rSet = buf[1].rSet;
		blockUp.slot.key = buf[1].key;	
	}
	
	void InsertLeaf( slotType  newslot, pointType & curP ) { // �s�W�b���`�I�W 
		for ( int i = MAX_DATA-1 ; i >= curP.pidx ; i-- ) {
			if ( i > curP.pidx ) { // �`�I���¸�Ʃ��k�䲾
				curP.pnode->data[i].rSet = curP.pnode->data[i-1].rSet;
				curP.pnode->data[i].key = curP.pnode->data[i-1].key;		
			}		
			
			else if ( i == curP.pidx ) {  // �x�s�s���
				curP.pnode->data[i].rSet = newslot.rSet;
				curP.pnode->data[i].key = newslot.key;
			}
			
			else {
				break;
			} 
		}	
	}
	
	void SearchPath( string newkey, stack <pointType> & path ) { // �j�M���| 
		pointType oneP;
		int pos;
		nodeType *walk = root;
		while ( walk != NULL ) {
			oneP.pnode = walk; // �������X�L���`�I 
 			for ( pos = 0 ; pos < MAX_DATA ; pos++ ) {
				
				// �S���ϥΪ��`�I��� �� �s����Ƥj��Τp���ª��`�I��� // 
				if ( !walk->data[pos].rSet.size() || 
				     ( newkey.compare( walk->data[pos].key)) < 0 ) {
					break; // ���U�@�h���X 
				}
				// �S���ϥΪ��`�I��� �� �s����Ƥj��Τp���ª��`�I��� //
				
				// �s����Ƶ����ª��`�I��� //  
				else if ( !newkey.compare(walk->data[pos].key)) {
					oneP.pidx = pos; // ������m 
					path.push(oneP); // �s�W���X�L�����| 
					return;
				}
				// �s����Ƶ����ª��`�I��� //
			}
			
			oneP.pidx = pos; // ������m  
			path.push(oneP); // �s�W���X�L�����| 
			walk = walk->child[pos]; // ���U�@�h���X����NULL  
		}
	} 
	
	void Insert23Tree( int iOFkey, string newkey ) { // �s�W�`�I�b23��W
		slotType newSlot;
		newSlot.rSet.push_back(iOFkey);
		newSlot.key = newkey; 
		if ( root == NULL ) { // �s�W�Ĥ@�����
			root = CreateNode( NULL, NULL, NULL, newSlot );
		}
		
		else { // �s�W�U�@�����
			stack <pointType> aPath; // �����j�M���|�W���`�I 
			pointType curP; // �̫�@�Ө��X���`�I( �n�s�W��ƪ���m ) 
			blockType blockUp; // �x�s�����`�I�᪺��� 
			SearchPath( newkey, aPath ); // �j�M���|
			if ( !aPath.empty() ) {
				curP = aPath.top(); // ���o�̫�@�Ө��X���`�I( �n�s�W��ƪ���m )
				// ���ƪ���ƪ�����J // 
				if ( ( curP.pnode->data[curP.pidx].rSet.size() ) && 
					 ( !newkey.compare(curP.pnode->data[curP.pidx].key) ) ) {
					curP.pnode->data[curP.pidx].rSet.push_back(iOFkey);	  	
				}
				// ���ƪ���ƪ�����J //
				
				// �ܤ֦��@�ӨS���ϥΪ���Ƹ`�I (�̥k��`�I) //  
				else if ( !curP.pnode->data[MAX_DATA-1].rSet.size() ) {
					InsertLeaf( newSlot, curP ); // �s�W�츭�`�I�W
				}
				// �ܤ֦��@�ӨS���ϥΪ���Ƹ`�I (�̥k��`�I) //
				
				else { // ���`�I����ƺ��F�A�������`�I
					SplitLeaf( newSlot, curP, blockUp );
				 	if ( curP.pnode->parent == NULL ) { // ��ڪ���ƺ��F�A������� 
						root = CreateRoot( curP.pnode, blockUp.link, blockUp.slot );
					}
					
					else {
						do {
							aPath.pop();
							curP = aPath.top();
							
							// �ܤ֦��@�ӨS���ϥΪ���Ƹ`�I (�̥k��`�I) //
							if ( !curP.pnode->data[MAX_DATA-1].rSet.size() ) {
								InsertNonLeaf( blockUp, curP ); // �s�W�b�����`�I
								break; 
							}
							// �ܤ֦��@�ӨS���ϥΪ���Ƹ`�I (�̥k��`�I) //
							
							else {
								SplitNonLeaf( blockUp, curP ); // ���������`�I
								if ( curP.pnode->parent == NULL ) { // ��ڪ���ƺ��F�A�������
									root = CreateRoot( curP.pnode, blockUp.link, blockUp.slot );
									break;
								}
							}
						} while( true );
					}
				}
			}
		}
	}
	
	void Show() {
		cout << "Tree Height = " << GetHeight( root ) << endl << endl;
		
		int index = 1;
		if ( ( !root->data[1].rSet.size() ) || ( root->data[0].rSet[0] <= root->data[1].rSet[0] ) ) {
			for ( int i = 0 ; i < MAX_DATA ; i++ ) {
				if ( root->data[i].rSet.size() ) {
					for ( int j = 0 ; j < root->data[i].rSet.size() ; j++ ) {
						for ( int k = 0 ; k < input.size() ; k++ ) {
							if ( input[k].index == root->data[i].rSet[j] ) {
								cout << index << ": " << "[" << root->data[i].rSet[j] << "] "
									 << input[k].school << ", " << input[k].major << ", "
									 << input[k].type << ", " << input[k].level << ", "
									 << input[k].student << endl;
								index++;
							}
						}		
					}
				}
			}		
		}
		
		else {
			for ( int i = MAX_DATA-1 ; i >= 0 ; i-- ) {
				if ( root->data[i].rSet.size() ) {
					for ( int j = 0 ; j < root->data[i].rSet.size() ; j++ ) {
						for ( int k = 0 ; k < input.size() ; k++ ) {
							if ( input[k].index == root->data[i].rSet[j] ) {
								cout << index << ": " << "[" << root->data[i].rSet[j] << "] "
									 << input[k].school << ", " << input[k].major << ", "
									 << input[k].type << ", " << input[k].level << ", "
									 << input[k].student << endl;
								index++;
							}
						}		
					}
				}
			}			
		}		
	}
	
	void Bulid23Tree() { // �إ�23�� 
		for ( int i = 0 ; i < input.size() ; i++ ) {
			Insert23Tree( i+1, input[i].school );
		}
		
		Show();
		Free23Tree(root);
		root = NULL;
	}
	
};



int main(void) {
	int command = -1;
	string num;
	Two_Three_Tree treeOf2_3;
	
	while ( command != 0 ) {
		cout << endl;
		cout << "*========================*\n" 
			 << "*  �п�ܥH�U��k����:   *\n"
			 << "*  1.�إ�2-3��.          *\n" 
			 << "*  2.�إ�AVL��.          *\n"
			 << "*  0.����.               *\n"
			 << "*========================*\n\n"
			 << "Answer > " ; 
		cin >> command;
		if ( command == 1 ) {		
			
			cout << endl ;
			cout << "�п�Jinput�ɮ�(example: 201, 202, ....) : " ;
			cin >> num;
			if ( treeOf2_3.Read( num ) ) {
				
				treeOf2_3.Bulid23Tree();
				cout << endl;
				treeOf2_3.Clear();
			}
		} 
		
		else if ( command == 2 ) {
			cout << endl;
			
		} 
		
		else if ( command != 0 ) {
			cout << endl << "Error !!! �п�J���T�����O!!!\n\n" ; 
		}
	}
	
	cout << endl << "����!" << endl; 
}
