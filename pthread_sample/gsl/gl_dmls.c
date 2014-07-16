#include "gsl.h"

void GL_DebugAssert(char *pFileName, GL_UINT32 dLineNumber, GL_UINT32 dExpressionEvaluation, char *pConditionText)
{
	if (dExpressionEvaluation == 0) {
		printf("ASSERT: %s  %d  %s\n", pFileName, dLineNumber, pConditionText);
		while (1) 
		;
	}
}

