#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#define ALPHABET_SIZE	26
#define MAX_WORD	1000
#define MAX_DOC 200

const char delim[]= ".,:;`/\"\'§ªº°'+-_(){}[]<>*&^%$#@!?~/|\\=1234567890 \t\n";
char palavras[10][MAX_WORD];
int resultado[MAX_DOC];

void strlwr(char* str) {
	size_t i;
	size_t len = strlen(str);

	for (i = 0; i < len; i++)
		str[i] = tolower((unsigned char) str[i]);
}

typedef struct pos{
	int value;
	struct pos *next;
} Pos;

typedef struct TrieTree {
	char valueChar;
	//char *word;
	int flag;
	int nDocs[MAX_DOC];
	//int n;
	Pos *p[MAX_DOC];
	struct TrieTree *v[ALPHABET_SIZE];
} tt;

tt* root = NULL;

tt* createRoot(){
	int i;
	tt* q=(tt*)malloc(sizeof(tt));
	if (q==NULL){
		puts("FALTOU MEMORIA!");
		return 0;
	}
	for (i=0; i<ALPHABET_SIZE; i++) q->v[i]=NULL;
	q->flag=0;
	for (i=0; i<MAX_DOC; i++) q->p[i]=NULL;
	return q;
}

tt* createNode(char carac){
	int i;
	tt* q=(tt*)malloc(sizeof(tt));
	if (q==NULL){
		puts("FALTOU MEMORIA!");
		return 0;
	}
	for (i=0; i<ALPHABET_SIZE; i++) q->v[i]=NULL;
	q->valueChar=carac;
	q->flag=0;
	for (i=0; i<MAX_DOC; i++) q->p[i]=NULL;
	return q;
}

void insertWord(char *palavra) {
	int i;
	if (root == NULL) root = createRoot();
	tt*current=root;
	for (i=0;i<strlen(palavra); ++i){
		char currentChar = tolower(palavra[i]);
		int index = currentChar-'a';
		assert(index>=0);
		if (current->v[index]!=NULL)
			current = current ->v[index];
		else{
			tt *new = createNode(currentChar);
			current->v[index]=new;
			current=new;
		}
		if (i==strlen(palavra)-1){
			current->flag=1;
			//current->word=(char*)calloc(MAX_WORD,sizeof(char));
			//strcpy(current->word,palavra);
		}
	}
}

tt* searchNode(char* word){
	tt* current = root;
	int i;
	//if (current==NULL) return current;
	for (i=0; i < strlen(word); ++i){
		char currentChar = tolower(word[i]);
		int index = currentChar - 'a';
		assert(index>=0);
		if (current->v[index]!=NULL)
			current=current->v[index];
		else
			return NULL;
		if (i==strlen(word)-1 && current->flag==0)
			return NULL;
	}
	return current;	
}

void insertValues(char *palavra, int doc, int posicao){
	tt* q = searchNode(palavra);
	if (!q){
		printf("FRASE NAO ENCONTRADA!\n");
		return;
	}
	/* INSERINDO INDICE DO DOCUMENTO */
	if (!q->nDocs[doc-1]){
		q->nDocs[doc-1]=doc;
		//q->n++;
	}
	/* INSERINDO POSICAO DA PALAVRA */
	Pos * novo = (Pos*)malloc(sizeof(Pos)) ;
	if (novo == NULL){
		puts("Faltou memoria!\n");
		return;
	}
	novo->value = posicao;
	novo->next = NULL;
	if (!q->p[doc-1]){
		q->p[doc-1]=novo;
	}
	else{ //if (q->p[doc-1]){
		Pos *b = q->p[doc-1];
		while (b->next) b=b->next;
		b->next=novo;
	}
}

/*void printIndice(tt* node, char *prefix){
	int i,j;
	char * currentString = (char*)malloc(sizeof(char));
	if (node==NULL) return;
	else if (node->flag==1){
		//printf("%s ",node->word);
		Pos *u;
		for (j=0; j<MAX_DOC; j++){
			u=node->p[j];
			if (node->nDocs[j]){
				printf("%d: ",node->nDocs[j]);
				while (u){
					printf("%d ",u->value);
					u = u->next;
				}
				puts("");
			}
		}
	}
	for (i=0; i < ALPHABET_SIZE; ++i){
		if (node->v[i]!=NULL){
			strcpy(currentString,prefix+node->v[i]->valueChar);
			printIndice(node->v[i], currentString);
		}
	}
}*/

void insertResult(int doc){
	if (!resultado[0]) resultado[0]=doc;
	else{
		int i;
		for (i=0; i<MAX_DOC; i++){
			if (resultado[i]==doc)
				return;
		}
		for (i=0; i<MAX_DOC; i++){
			if (resultado[i]==0)
				break;
		}
		resultado[i]=doc;
	}
}

void busca_exata(){
	int i, j, cont, posicao;
	tt * w1, *w2;
	for (i = 0; i<MAX_DOC ; i++){
		cont=0;
		j=0;
		posicao=0;
		while (strcmp(palavras[j+1],"")){
			w1 = searchNode(palavras[j]);
			w2 = searchNode(palavras[j+1]);
			if ( w1->nDocs[i] && w2->nDocs[i] && (w1->nDocs[i] == w2->nDocs[i]) ){
				Pos * p1, *p2;
				p1 = w1 -> p[i];
				while (p1){
					p2=w2->p[i];
					while (p2->next && p2->value < p1->value) p2 = p2->next;
					if ( ( (p1->value+1 == p2->value) && posicao==0 ) || ((p1->value+1 == p2->value) && (posicao+1==p2->value)) ){
					//if (p1->value+1 == p2->value){
						cont++;
						posicao=p2->value;
						//p1= p1->next;
						break;
					}
					else {
						p1=p1->next;
					}
				}
			}
			j++;
		}
		if (!w1){ //só tem uma palavra na entrada
			w1=searchNode(palavras[0]);
			if (w1->nDocs[i]) insertResult(w1->nDocs[i]);
		}
		else if (w1 && j==cont) insertResult(w1->nDocs[i]);
	}
}

int main(int argc, char **argv) {
	if (argc == 1) {
		printf("Sintaxe: ./%s arq1 arq2 arq3 ... arqN\n", argv[0]);
		return 0;
	}
	
	int i, j, erro;
	
	for (i=1; i<argc; i++){
		struct stat arquivos;
		stat(argv[i],&arquivos);
		char *buffer = (char*)calloc(1,arquivos.st_size);
		FILE* file = fopen(argv[i],"r");
		if (file == NULL) {
			printf("Erro, nao foi possivel abrir o arquivo: %s\n",argv[i]);
		}
		fread(buffer, arquivos.st_size, 1, file);
		fclose(file);
		if (strcmp(buffer,"")){
			char *w = strtok(buffer,delim);
			j=1;
			while (w!=NULL){
				strlwr(w);
				insertWord(w);
				insertValues(w,i,j);
				j++;
				w=strtok(NULL,delim);
			}
		}
		free(buffer);
	}
	
	char entrada[MAX_WORD];
	while (fgets(entrada, MAX_WORD, stdin)){
		erro=0;
		if (strcmp(entrada,"\n")){
			for (i=0; i<10; i++) memset(&palavras[i],0,MAX_WORD);
			for (i=0; i<MAX_DOC; i++) resultado[i]=0;
			char *word = strtok(entrada, delim);
			i=0;
			while (word){
				strlwr(word);
				if (searchNode(word)){
					strcpy(palavras[i],word);
					i++;
				}
				else{
					erro=1;
					printf("FRASE NAO ENCONTRADA!\n");
					break;
				}
				word=strtok(NULL,delim);
			}
			if (!erro){
				busca_exata();
				if (!resultado[0]) printf("FRASE NAO ENCONTRADA!\n");
				else {
					for (i=0; i<MAX_DOC; i++){
						if (resultado[i]){
							if (!resultado[i+1]) printf("%s\n",argv[resultado[i]]);
							else printf("%s ",argv[resultado[i]]);
						}
					}
				}
			}
		}
	}
	return 0;
}
