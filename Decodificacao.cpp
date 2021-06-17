#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define TAM_PALA 10
#define TAM_CODIGO 10
#define TAM_CODIFICACAO 256
#define ARQ_TAB "TabFreq.dat"
#define ARQ_CODIGO "Codigo Huffman.txt"

/* 

ALUNO: ITALO CESAR PIOVAN ROCHA
*/

///ESTRUTURAS
struct Tabela
{
	char palavra[TAM_PALA],huffman[TAM_CODIGO];
	int frequencia,simbolo;	
		
};typedef struct Tabela tab;

struct Tree
{
	int simbolo;
	int freq;
	Tree *esq,*dir;  
	
};typedef struct Tree tree;

/// FIM ESTRUTURAS

int BuscaPalavra(Tabela t[],char pala[],int tam,int simbolo)
{
	int i=0;
	
	if(simbolo == -1)// procura pelo simbolo
		while(i < tam && strcmp(t[i].palavra,pala) != 0)
			i++;
	else // procura pela palavra
		while(i < tam && t[i].simbolo != simbolo)
			i++;
	
	if(!strcmp(t[i].palavra,pala) || simbolo == t[i].simbolo)
		return i;
	else
		return -1;		
}

Tree *CriaNO(int simbolo,int freq,Tree *esq,Tree *dir)
{
	Tree *no = (Tree*)malloc(sizeof(Tree)); 
	no->esq = esq;
	no->dir = dir;
	no->freq = freq;
	no->simbolo = simbolo;
	return no;
}

void GeraTree(Tree **raiz,Tabela t[],int TL)
{
	char codigo[TAM_CODIGO]="";
	Tree *aux;
	
	*raiz = CriaNO(-1,-1,NULL,NULL);
	
	for(int i=0;i< TL ;i++)
	{
		aux = *raiz;
		strcpy(codigo,t[i].huffman);
		
		for(int j=0;j< strlen(codigo);j++)
		{
			if(codigo[j] == '0')
			{
				if(aux->esq == NULL)
					aux->esq = CriaNO(-1,-1,NULL,NULL);
				aux = aux->esq;	
			}	
			else
			{
				if(aux->dir == NULL)
					aux->dir = CriaNO(-1,-1,NULL,NULL);
				aux = aux->dir;	
			}	
		}
		if(aux != NULL)
		{
			aux->simbolo = t[i].simbolo;
			aux->freq = t[i].frequencia;
		}
	}	
}

void Exibe(Tabela t[],int TL,Tree *raiz)
{
	FILE *pCod = fopen(ARQ_CODIGO,"r");
	Tree *aux = raiz;
	char codigo[TAM_CODIFICACAO]="";
	
	for(int i=0;!feof(pCod);i++)
		codigo[i] = getc(pCod);	
		
	for(int j=0; j< strlen(codigo);j++)
	{
		if(aux->esq == NULL && aux->dir == NULL)
		{
			printf("%s",t[BuscaPalavra(t,"",TL,aux->simbolo)].palavra);
			aux = raiz;
		}
		
		if(codigo[j] == '0')
			aux = aux->esq;
		else
			aux = aux->dir;	
	}
	fclose(pCod);
}

void Decodifica(Tabela *t)
{
	int TL = 0;
	Tree *raiz=NULL;
	
	FILE *pTab = fopen(ARQ_TAB,"rb");	
	
	while(!feof(pTab))
		fread(&t[TL++],sizeof(Tabela),1,pTab);
		
	TL--;
	GeraTree(&raiz,t,TL);
	Exibe(t,TL,raiz);
	
	fclose(pTab);
}

int Tam_Tabela()
{
	int tam =0;
	FILE *pTab = fopen(ARQ_TAB,"rb");
	
	if(pTab!= NULL)
	{
		fseek(pTab,0, SEEK_END);
		tam = ftell(pTab)/sizeof(Tabela);
	}	
	fclose(pTab);
	return tam;
}

int main()
{
	Tabela t[Tam_Tabela()];
	Decodifica(t);
}
