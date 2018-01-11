#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "symtable.h"
#include "type.h"
bool fieldEq(FieldList a,FieldList b){
	if(!a&&!b)
		return true;
	if((a&&!b)||(b&&!a))
		return false;
	
	return typeEq(a->type,b->type) && fieldEq(a->tail,b->tail) ;

}

bool typeEq(Type a,Type b){
	//Structural Equivalence
	if(!a&&!b)
		return true;
	if((a&&!b)||(b&&!a))
		return false;
	if(a->kind == b->kind){
		switch(a->kind){
			case BASIC:if(a->basic==b->basic)	
						   return true;
					   else
						   return false;
			case ARRAY:return typeEq(a->array.elem,b->array.elem);
			case STRUCTURE:return fieldEq(a->structure,b->structure);
			
		}
	}
	
	return false;

}

bool isInt(Type a){
	if(!a)
		return false;
	return a->kind == BASIC && a->basic == INT;
}

bool isFloat(Type a){
	if(!a)
		return false;
	return a->kind == BASIC && a->basic == FLOAT;
}

bool isStruct(Type a){
	if(!a)
		return false;
	return a->kind == STRUCTURE;	
}

Type isField(FieldList field,const char *name){
	FieldList p = field;
	while(p){
		if(strcmp(p->name,name)==0){
			return p->type;
		}
		p = p->tail;
	}
	return NULL;
}	

Type isArray(Type a){
	if(!a)
		return NULL;
	if(a->kind == ARRAY)
		return a->array.elem;
	return NULL;
}

int sizeofType(Type a){
	switch(a->kind){
		case BASIC : return 4;
		case ARRAY :return a->array.size * sizeofType(a->array.elem);
		case STRUCTURE : 
		{
			int size = 0;
			FieldList p = a->structure;
			while(p){
				size += sizeofType(p->type);
				p = p->tail ;
			}
			return size;
		}
		default : return -1;
	}
}
Type typeofNode(Node *p){
    if(strcmp(p->child[0]->symbol,"ID")==0){
        char *v = p->child[0]->lexeme;
        Type type = NULL;
        searchSymTable(v,Variable,(void**)&type,1);
        assert(type);
        return type;
    }
    else if(strcmp(p->child[1]->symbol,"DOT")==0){
        char *id = p->child[2]->lexeme;
        FieldList field  = typeofNode(p->child[0])->structure;
        while(strcmp(field->name,id)!=0){
            field = field->tail;
        }
        return field->type;
    }
    else /*if(p->num_of_child==4)*/{
        return typeofNode(p->child[0])->array.elem;
    }
}


