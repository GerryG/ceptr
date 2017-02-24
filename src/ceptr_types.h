#ifndef _CEPTR_TYPES_H
#define _CEPTR_TYPES_H

// NOTE: the actual values of the types matter because they must match the order they show
// up in the definition trees
enum SemanticTypes {
  SEM_TYPE_STRUCTURE=1,
  SEM_TYPE_SYMBOL,
  SEM_TYPE_PROCESS,
  SEM_TYPE_RECEPTOR,
  SEM_TYPE_PROTOCOL
};
#define is_symbol(sem_id) ((sem_id).semtype == SEM_TYPE_SYMBOL)
#define is_process(sem_id) ((sem_id).semtype == SEM_TYPE_PROCESS)
#define is_structure(sem_id) ((sem_id).semtype == SEM_TYPE_STRUCTURE)
#define is_protocol(sem_id) ((sem_id).semtype == SEM_TYPE_PROTOCOL)
#define is_receptor(sem_id) ((sem_id).semtype == SEM_TYPE_RECEPTOR)

typedef uint32_t Context;     // 4G types of receptors
typedef uint16_t SemanticType; // 256 types of semantic things (but using 2 bytes for struct alignment!)
typedef uint16_t SemanticAddr;// 64K types of symbols/structure per receptor

/*
 * These aren the internal symbols of Ceptr used where-ever a semantic tag is
 * needed. I don't think we want two symbols with the same name in different contexts
 * but it isn't totally clear what context is semantically. Sort of like user/system
 * Likewise with types, so either (don't like) context determines or maybe prefixes
 * for the enum values?
 * so :sys_structure_Name for a system context structure? :Name for a plain (non-sys)
 * symbol and so on.
 */
// bit-packing, ugly and unnecessary
typedef struct SemanticID {
    Context context;
    SemanticType semtype;
    SemanticAddr id;
    //c    uint8_t _reserved; // add an extra 8 bits to make this a 64bit structure.
} SemanticID;

// creating aliases for SemanticIDs as hints for use.
typedef SemanticID Symbol;
typedef SemanticID Structure;
typedef SemanticID Process;
typedef SemanticID Protocol;
typedef SemanticID Recept;


// ** types for matrix trees
// exactly what are matrix types? Hide/remove this detail.
typedef uint16_t Mlevel;
typedef uint32_t Mindex;
typedef uint32_t Mmagic;

typedef struct Node {
    Symbol symbol;
    Mindex parenti;
    uint32_t flags;
    uint32_t cur_child;  // for active run-trees
    size_t size;
    void *surface;  // this item must be last!!!
} Node, N; // convert to longer names and remove

typedef struct Level {
    Mindex nodes;
    Node *nP;
} Level, L; // convert to longer names and remove

typedef struct Mtree {
    Mmagic magic;
    Mlevel levels;
    Level *lP;
} Mtree, M; // convert to longer names and remove

// node entries are fixed size but the surface when serialized is an offset
// in the blob not a pointer
// ugly appearances of low level format details everywhere, remove
#define SERIALIZED_NODE_SIZE (sizeof(N)-sizeof(void *)+sizeof(size_t))
#define SERIALIZED_LEVEL_SIZE(l) (sizeof(Mindex)+SERIALIZED_NODE_SIZE*l->nodes)
#define SERIALIZED_HEADER_SIZE(levels) (sizeof(S)+sizeof(uint32_t)*(levels))

typedef struct SerialMtree {
    Mmagic magic;
    size_t total_size;
    Mlevel levels;
    uint32_t blob_offset;
    uint32_t level_offsets[];
} SerialMtree, S;

// I guess this is what is meant by "matrix", implicitely this is a breadth first
// representation: 0: (root node), 1: (children) 2: (children's childres)
// This is just about the addressing scheme of serialized data, we don't need it
#define NULL_ADDR -1
typedef struct Maddr {
    Mlevel l;
    Mindex i;
} Maddr;

// ** generic tree type defs
typedef struct HashTree {
    Mtree *m;
    Maddr a;
} HashTree, H;

// I don't see that this is even used, maybe in magic numbers?
enum treeImplementations {ptrImpl=0xfffffffe,matrixImpl=0xffffffff};
#define FIRST_TREE_IMPL_TYPE ptrImpl
#define LAST_TREE_IMPL_TYPE matrixImpl


// ** types for pointer trees
typedef struct Tstruct {
    uint32_t child_count;
    struct TreeNode *parent;
    struct TreeNode **children;
} Tstruct;

typedef struct Tcontents {
    Symbol symbol;
    size_t size;
    void *surface;
} Tcontents;

typedef struct Tcontext {
    uint32_t flags;
} Tcontext;

/**
 * A TreeNode:
 *  structure: relative nodes
 *    children[]
 *    parent
 *  context: ? logical context -- detail what is 'contextualized' by a node's context
 *  contents: The 'value' of this node
 *    symbol: I think this is always the label of the link from the parent to this child
 *    surface: I think this is in-effect the 'type' of this node, should be the
 *             semantic id of the type of the node (I think)
 *
 */
typedef struct TreeNode {
    Tstruct structure;
    Tcontext context;
    Tcontents contents;
} TreeNode, T;

#define RUN_TREE_NOT_EVAULATED       0
#define RUN_TREE_EVALUATED           0xffffffff

/**
 * A run tree node
 *  just like T nodes but with run state data appended
 * used in rclone
 **/
typedef struct RunTree {
    Tstruct structure;
    Tcontext context;
    Tcontents contents;
    uint32_t cur_child;
} RunTree, rT;

// macro helper to get at the cur_child element of a run-tree node when given a regular
// node (does the casting to make code look cleaner)
#define rt_cur_child(tP) (((rT *)tP)->cur_child)

typedef uint32_t TreeHash; // Maybe call this TreeKey ? Don't confuse with Hash type

// ** types for labels
typedef uint32_t Label;

/**
 * An element in the label table.
 *
 */
typedef struct table_elem {
    UT_hash_handle hh; ///< makes this structure hashable using the uthash library
    Label label;       ///< semantic key
    int path_s;        ///< first int of the path to the labeled item in the Receptor tree
} table_elem, *LabelTable;

// for now store instances in an INSTANCES semantic tree
typedef TreeNode *Instances;
typedef Instances *TreeNodes;  // this might be clearer

typedef struct ConversationState ConversationState;
struct ConversationState {
    TreeNode *converse_pointer;///< pointer to the CONVERSE instruction in the run tree
    TreeNode *cid;  ///< pointer to CONVERSATION_IDENT in receptors CONVERSATIONS tree
    ConversationState *next;
};

// ** types for processing
// run-tree context
typedef struct Runner Runner, R; // switch to longer name
struct Runner {
    int id;           ///< the process id this context exists in
    int err;          ///< process error value
    int state;        ///< process state machine state
    TreeNode *run_tree; ///< pointer to the root of the run_tree
    TreeNode *node_pointer;  ///< pointer to the tree node to execute next
    TreeNode *parent; ///< node_pointer's parent      (cached here for efficiency)
    int idx;          ///< node pointers child index  (cached here for efficiency)
    Runner *caller;  ///< a pointer to the context that invoked this run-tree/context
    Runner *callee;  ///< a pointer to the context we've invoked
    TreeNode *sem_map; ///< semantic map in effect for this context
    ConversationState *conversation;  ///< record of the conversation state active in this context frame
};

// ** structure to hold in process accounting
typedef struct Accounting Accounting;
struct Accounting {
    uint64_t elapsed_time;
};

// Processing Queue element
typedef struct QueElement QueElement, Qe;
struct QueElement {
    int id;
    Runner *context;
    Accounting accounts;
    QueElement *next;
    QueElement *prev;
};

typedef struct ReceptorAddress {
    int addr;
} ReceptorAddress;

typedef struct Receptor Receptor; // about to use it, define this now

// Processing Queue structure
typedef struct Queue {
    Receptor *ceptr;         ///< back-pointer to receptor in which this Q is running (for defs and more)
    int contexts_count;  ///< number of active processes
    QueElement *active;          ///< active processes
    QueElement *completed;       ///< completed processes (pending cleanup)
    QueElement *blocked;         ///< blocked processes
    pthread_mutex_t mutex;
} Queue, Q;

// SemTable structures
typedef struct ContextStore {
    TreeNode *definitions;
    //LabelTable table;    ///< the label table for this context?
} ContextStore;

//@todo convert to malloc
#define MAX_CONTEXTS 100
typedef struct SemTable {
    int contexts;
    ContextStore stores[MAX_CONTEXTS];
} SemTable;


// ** types for receptors
enum ReceptorStates {Alive=0,Dead};

/**
   A Receptor is a semantic tree, pointed to by root, but we also create c struct for
   faster access to some parts of the tree, and to hold non-tree data, like the label
   table.
*/
// class Receptor # will be a key class in any implementation
struct Receptor {
    TreeNode *root;      ///< RECEPTOR_INSTANCE semantic tree
    Context parent;      ///< the context this receptor's definition lives in
    Context context;     ///< the context this receptor's definition creates
    ReceptorAddress addr;///< the address by which to get messages to this receptor instance
    SemTable *sem;       ///< pointer back to the genotype table for this receptor's vmhost instance
    TreeNode *flux;      ///< pointer for quick access to the flux
    TreeNode *pending_signals;
    TreeNode *pending_responses;
    TreeNode *conversations;
    pthread_mutex_t pending_signals_mutex;
    pthread_mutex_t pending_responses_mutex;
    Instances instances; ///< the instances store
    Queue *q;            ///< process queue
    int state;           ///< state information about the receptor that the vmhost manages
    TreeNode *edge;      ///< data store for edge receptors
};

typedef struct UUIDt {
    uint64_t data;
    uint64_t time;
} UUIDt;

// aspects appear on either side of the membrane
enum AspectType {EXTERNAL_ASPECT=0,INTERNAL_ASPECT};
typedef Symbol Aspect;  //aspects are identified by a semantic Symbol identifier

/**
 * An eXistence Address consists of the semantic id (Symbol) and an address.
 * address of what? A channel maybe?
 */
typedef struct Xaddr {
    Symbol symbol;
    int addr;
} Xaddr;

typedef int Error;

// ** types for scapes

/**
 * An element in the scape key value pair store
 */
typedef struct scape_elem {
    TreeHash key;            ///< hash key of the tree that maps to a given data value
    Xaddr value;             ///< instance of data_source pointed to by the key
    UT_hash_handle hh;       ///< makes this structure hashable using the uthash library
} scape_elem;
typedef scape_elem *ScapeData;

/**
 * A scape provides indexed, i.e. random access to data sources.  The key source is
 * usually a sub-portion of a the data source, i.e. if the data source is a PROFILE
 * the key_source might be a FIRST_NAME within the profile
 */
typedef struct Scape {
    Symbol key_source;
    Symbol data_source;
    ScapeData data;      ///< the scape data store (hash table)
} Scape;

#endif
