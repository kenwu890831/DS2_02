// 10724128 吳宇哲 10727104 王子威 

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
		vector <int> rSet; // 紀錄相同鍵值的資料 
		string key; // 鍵值
	}slotType;
	
	typedef struct nT {
		slotType data[MAX_DATA]; // 結點資料
		nT *child[MAX_CHILD]; // 每一個節點最多三個子節點 
		nT *parent; // 指向父節點 
	}nodeType;
	
	typedef struct pointT {  
		nodeType *pnode; // 搜尋路徑上的節點
		int pidx; // 路徑節點方向 
	}pointType;
	
	typedef struct bT {
		slotType slot; 
		nodeType *link; // 指向右節點 
	}blockType;
	
	fstream file; // 檔案處理
	nodeType *root; // 指向root  
	vector <collegeType> input; // 動態陣列
	int height; // 樹高
	int nodeNum; // 節點數 
	
public:
	Two_Three_Tree() { // constructor
		root = NULL;
		height = 0;
	}

	bool Read( string num ) { // 讀檔案 
		cout << endl;
		string filename,line,cut;
		filename = "input"+ num + ".txt";
		file.open( filename.c_str(), ios::in );
		if ( !file ) {
			cout << "無指定的檔案名稱: " << filename << endl << endl;
			return false;
		}
		else {
			/* 把前三行讀掉 */ 
			getline(file,line);
			getline(file,line);
			getline(file,line);
			/* 把前三行讀掉 */
			collegeType buffer;
			int i = 1; // 編號 
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
	
	void Free23Tree( nodeType *root ) { // 刪除節點 
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

	void Clear() { // 清除vector 
		input.clear();
		Free23Tree(root);
		root = NULL;
	}
	
	
	int GetHeight( nodeType *walk ) { // 取得樹高 
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
	
	// 新增節點 // 
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
	// 新增節點 //
	
	// 新增新的樹根 // 
	nodeType* CreateRoot( nodeType *pNode, nodeType *link, slotType & slot ) {
		nodeType *newRoot = CreateNode( pNode, link, NULL, slot );
		pNode->parent = newRoot;
		link->parent = newRoot;
		return newRoot;	
	}
	// 新增新的樹根 //
	 
	void SplitNonLeaf( blockType & blockUp, pointType & curP ) { // 分離內部節點 
		slotType buf[MAX_CHILD];
		nodeType *ptr[MAX_CHILD+1];  
		nodeType *rightNode; // 新的右子樹 
		int idx = 0;
		for ( int i = 0 ; i < MAX_CHILD ; i++ ) { // 排序 
			buf[i].rSet = ( i == curP.pidx ) ? blockUp.slot.rSet : curP.pnode->data[idx].rSet;
			buf[i].key = ( i == curP.pidx ) ? blockUp.slot.key : curP.pnode->data[idx++].key;
		}
	
		curP.pnode->data[0].rSet = buf[0].rSet;
		curP.pnode->data[0].key = buf[0].key;
		for ( int i = 1 ; i < MAX_DATA ; i++ ) { // 留下最小的資料 
			curP.pnode->data[i].rSet.clear();
		}
				
		for ( int i = 0 ; i < MAX_CHILD ; i++ ) { // 找到當前節點的所有子節點 
			ptr[i] = curP.pnode->child[i];
		}
		
		// 所有子節點(從最大的)開始和blockUp.link比大小 //  
		for ( int i = MAX_CHILD - 1 ; i >= 0 ; i-- ) { 
			if ( blockUp.link->data[0].key.compare( ptr[i]->data[0].key ) > 0 ) {
				ptr[3] = blockUp.link;
				if ( i == MAX_CHILD - 1 ) { // blockUp.link是最大的接在新的右子樹的右節點 
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[1];
					curP.pnode->child[2] = NULL;
					rightNode = CreateNode( ptr[2], ptr[3], 
											curP.pnode->parent, buf[2]);
					break;
				}
				
				else if ( i == MAX_CHILD - 2 ) { // blockUp.link第二大接在新的右子樹的左節點
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[1];
					curP.pnode->child[2] = NULL;
					rightNode = CreateNode( ptr[3], ptr[2], 
											curP.pnode->parent, buf[2]);
					break;					
				}
				
				else if ( i == MAX_CHILD - 3 ) { // blockUp.link第三大接在當前節點的右節點 
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[3];
					curP.pnode->child[2] = NULL;
					rightNode = CreateNode( ptr[1], ptr[2], 
											curP.pnode->parent, buf[2]);
					break;						
				}
			}
		}
		
		// 所有子節點(從最大的)開始和blockUp.link比大小 //
	
		// 更新blockUp //  
		blockUp.slot.rSet = buf[1].rSet;  
		blockUp.slot.key = buf[1].key;
		blockUp.link = rightNode;
		// 更新blockUp //
	}
	
	void InsertNonLeaf( const blockType & blockUp, pointType & curP ) { // 新增內部節點
		nodeType *ptr[MAX_CHILD];
		for ( int i = MAX_DATA-1 ; i >= curP.pidx ; i-- ) {
			if ( i > curP.pidx ) { // 節點內舊資料往右邊移
				curP.pnode->data[i].rSet = curP.pnode->data[i-1].rSet;
				curP.pnode->data[i].key = curP.pnode->data[i-1].key;
			}		
			
			else if ( i == curP.pidx ) {  // 儲存新資料
				curP.pnode->data[i].rSet = blockUp.slot.rSet;
				curP.pnode->data[i].key = blockUp.slot.key;
			}
			
			else {
				break;
			}
		}	
		
		for ( int i = 0 ; i < MAX_CHILD - 1 ; i++ ) {  // 找到當前節點的所有子節點
			ptr[i] = curP.pnode->child[i];
		}
		
		// 所有子節點(從最大的)開始和blockUp.link比大小 //
		for ( int i = MAX_CHILD - 2 ; i >= 0 ; i-- ) {
			if ( blockUp.link->data[0].key.compare( ptr[i]->data[0].key ) > 0 ) {
				ptr[3] = blockUp.link;
				if ( i == MAX_CHILD - 2 ) { // blockUp.link是最大的接在當前節點的右節點
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[1];
					curP.pnode->child[2] = ptr[3];
					break;
				}
				
				else if ( i == MAX_CHILD - 3 ) { // blockUp.link是第二大的接在當前節點的中間節點 
 					curP.pnode->child[0] = ptr[0];
					curP.pnode->child[1] = ptr[3];
					curP.pnode->child[2] = ptr[1];
					break;					
				}
				
			}
		}
		// 所有子節點(從最大的)開始和blockUp.link比大小 //		
	}
	
	void SplitLeaf( slotType & newslot, pointType & curP, blockType & blockUp ) { // 分離葉節點 
		slotType buf[MAX_CHILD];
		int idx = 0; 
		for ( int i = 0 ; i < MAX_CHILD ; i++ ) { // 排序 
			buf[i].rSet = ( i == curP.pidx ) ? newslot.rSet : curP.pnode->data[idx].rSet;
			buf[i].key = ( i == curP.pidx ) ? newslot.key : curP.pnode->data[idx++].key;
		} 
		
		curP.pnode->data[0].rSet = buf[0].rSet;
		curP.pnode->data[0].key = buf[0].key;
		for ( int i = 1 ; i < MAX_DATA ; i++ ) { // 留下最小的資料 
			curP.pnode->data[i].rSet.clear();
		}		
		
		blockUp.link = CreateNode( NULL, NULL, curP.pnode->parent, buf[2]); 
		blockUp.slot.rSet = buf[1].rSet;
		blockUp.slot.key = buf[1].key;	
	}
	
	void InsertLeaf( slotType  newslot, pointType & curP ) { // 新增在葉節點上 
		for ( int i = MAX_DATA-1 ; i >= curP.pidx ; i-- ) {
			if ( i > curP.pidx ) { // 節點內舊資料往右邊移
				curP.pnode->data[i].rSet = curP.pnode->data[i-1].rSet;
				curP.pnode->data[i].key = curP.pnode->data[i-1].key;		
			}		
			
			else if ( i == curP.pidx ) {  // 儲存新資料
				curP.pnode->data[i].rSet = newslot.rSet;
				curP.pnode->data[i].key = newslot.key;
			}
			
			else {
				break;
			} 
		}	
	}
	
	void SearchPath( string newkey, stack <pointType> & path ) { // 搜尋路徑 
		pointType oneP;
		int pos;
		nodeType *walk = root;
		while ( walk != NULL ) {
			oneP.pnode = walk; // 紀錄走訪過的節點 
 			for ( pos = 0 ; pos < MAX_DATA ; pos++ ) {
				
				// 沒有使用的節點資料 或 新的資料大於或小於舊的節點資料 // 
				if ( !walk->data[pos].rSet.size() || 
				     ( newkey.compare( walk->data[pos].key)) < 0 ) {
					break; // 往下一層走訪 
				}
				// 沒有使用的節點資料 或 新的資料大於或小於舊的節點資料 //
				
				// 新的資料等於舊的節點資料 //  
				else if ( !newkey.compare(walk->data[pos].key)) {
					oneP.pidx = pos; // 紀錄位置 
					path.push(oneP); // 新增走訪過的路徑 
					return;
				}
				// 新的資料等於舊的節點資料 //
			}
			
			oneP.pidx = pos; // 紀錄位置  
			path.push(oneP); // 新增走訪過的路徑 
			walk = walk->child[pos]; // 往下一層走訪直到NULL  
		}
	} 
	
	void Insert23Tree( int iOFkey, string newkey ) { // 新增節點在23樹上
		slotType newSlot;
		newSlot.rSet.push_back(iOFkey);
		newSlot.key = newkey; 
		if ( root == NULL ) { // 新增第一筆資料
			root = CreateNode( NULL, NULL, NULL, newSlot );
		}
		
		else { // 新增下一筆資料
			stack <pointType> aPath; // 紀錄搜尋路徑上的節點 
			pointType curP; // 最後一個走訪的節點( 要新增資料的位置 ) 
			blockType blockUp; // 儲存分離節點後的資料 
			SearchPath( newkey, aPath ); // 搜尋路徑
			if ( !aPath.empty() ) {
				curP = aPath.top(); // 取得最後一個走訪的節點( 要新增資料的位置 )
				// 重複的資料直接放入 // 
				if ( ( curP.pnode->data[curP.pidx].rSet.size() ) && 
					 ( !newkey.compare(curP.pnode->data[curP.pidx].key) ) ) {
					curP.pnode->data[curP.pidx].rSet.push_back(iOFkey);	  	
				}
				// 重複的資料直接放入 //
				
				// 至少有一個沒有使用的資料節點 (最右邊節點) //  
				else if ( !curP.pnode->data[MAX_DATA-1].rSet.size() ) {
					InsertLeaf( newSlot, curP ); // 新增到葉節點上
				}
				// 至少有一個沒有使用的資料節點 (最右邊節點) //
				
				else { // 葉節點的資料滿了，分離葉節點
					SplitLeaf( newSlot, curP, blockUp );
				 	if ( curP.pnode->parent == NULL ) { // 樹根的資料滿了，分離樹根 
						root = CreateRoot( curP.pnode, blockUp.link, blockUp.slot );
					}
					
					else {
						do {
							aPath.pop();
							curP = aPath.top();
							
							// 至少有一個沒有使用的資料節點 (最右邊節點) //
							if ( !curP.pnode->data[MAX_DATA-1].rSet.size() ) {
								InsertNonLeaf( blockUp, curP ); // 新增在內部節點
								break; 
							}
							// 至少有一個沒有使用的資料節點 (最右邊節點) //
							
							else {
								SplitNonLeaf( blockUp, curP ); // 分離內部節點
								if ( curP.pnode->parent == NULL ) { // 樹根的資料滿了，分離樹根
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
	
	void Bulid23Tree() { // 建立23樹 
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
			 << "*  請選擇以下方法執行:   *\n"
			 << "*  1.建立2-3樹.          *\n" 
			 << "*  2.建立AVL樹.          *\n"
			 << "*  0.結束.               *\n"
			 << "*========================*\n\n"
			 << "Answer > " ; 
		cin >> command;
		if ( command == 1 ) {		
			
			cout << endl ;
			cout << "請輸入input檔案(example: 201, 202, ....) : " ;
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
			cout << endl << "Error !!! 請輸入正確的指令!!!\n\n" ; 
		}
	}
	
	cout << endl << "結束!" << endl; 
}
