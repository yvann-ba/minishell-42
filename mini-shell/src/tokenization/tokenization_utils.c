#include "../minishell.h"

int	is_special_char(char c)
{
	return (c == '>' || c == '<' || c == '|');
}

static	char	*process_quoted_content(const char *quoted_part,
					char quote_type, t_minishell *shell)
{
	char	*value;

	if (quote_type == '"')
		value = substitute_env_vars(quoted_part, shell->env, shell);
	else
		value = ft_strdup(quoted_part);
	return (value);
}

void add_quoted_token(const char **input, t_token **head, char quote_type, t_minishell *shell) {
    const char *start;
    size_t len = 0;
    char *quoted_part;

    (*input)++;
    start = *input;

    // Count the length until the closing quote or end of string
    while ((*input)[len] && (*input)[len] != quote_type) {
        len++;
    }

    // Handle empty quotes
    if (len == 0) {
        add_token(head, init_token(TOKEN_WORD, ""));
    } else {
        quoted_part = ft_strndup(start, len);
        char *value = process_quoted_content(quoted_part, quote_type, shell);
        add_token(head, init_token(TOKEN_WORD, value));
        free(quoted_part);
        free(value);
    }

    // Move past the closing quote if it exists
    if ((*input)[len] == quote_type) {
        *input += len;
    } else {
        ft_printf("minishell: syntax error: missing closing quote '%c'\n", quote_type);
        free_tokens(head); // Freeing all tokens because of syntax error
    }
}


void	handle_quotes(const char **input, t_token **head,
			t_minishell *shell, const char **start)
{
	char	*word;

	if (**input == '\'' || **input == '"')
	{
		if (*input != *start)
		{
			word = ft_strndup(*start, (size_t)(*input - *start));
			add_token(head, init_token(TOKEN_WORD, word));
			free(word);
		}
		add_quoted_token(input, head, **input, shell);
		*start = *input;
	}
	else
		(*input)++;
}

void	check_space_after_token(const char **input, t_token**head,
			t_minishell *shell)
{
	if (ft_isspace(**input) && (*head)->next && *(*input + 1) != '\0')
	{
		while (ft_isspace(*(*input + 1)))
			(*input)++;
		shell->space_flag = 1;
	}
}
