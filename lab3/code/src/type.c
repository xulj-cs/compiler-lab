#include <type.h>
#include <string.h>
#include <stdlib.h>
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

