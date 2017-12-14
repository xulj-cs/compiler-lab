#ifndef _SEMANTICANALYSIS_H_
#define _SEMANTICANALYSIS_H_

#define SERROR(i) semanticError(i,p->lineno)
extern int SERROR;
void semanticError(int,int);
void semanticAnalysis(Node *);
#endif
