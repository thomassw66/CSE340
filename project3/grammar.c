#define MAX_ID 1000
#define MAX_RULES 1000

typedef ID int 

struct Grammar {
	ID ids[MAX_ID];
	Rule rules[MAX_RULES];
	int id_count;
	int rule_count;	
};

struct Rule {
	ID lhs;
	ListNode lhs;
};

struct ListNode {
	ID id;
	ListNode next;	
};

int main (char ** argv, int argc) 
{
	
	return 0;
}