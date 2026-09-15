#include "ast/Environment.hpp"
#include <cstdio>


AST_USING_NAMESPACE

int main()
{
    printf("is CI: %d\n", aIsCI());
    printf("is Github CI: %d\n", aIsGithubCI());
    printf("is Gitlab CI: %d\n", aIsGitlabCI());
    printf("terminal support color: %d\n", aTerminalSupportColor());
    printf("terminal width: %d\n", aTerminalWidth());
    printf("stdin is terminal: %d\n", aIsTerminal(stdin));
    printf("stdout is terminal: %d\n", aIsTerminal(stdout));
    printf("stderr is terminal: %d\n", aIsTerminal(stderr));
    printf("stdin is file: %d\n", aIsFile(stdin));
    printf("stdout is file: %d\n", aIsFile(stdout));
    printf("stderr is file: %d\n", aIsFile(stderr));
    return 0;
}