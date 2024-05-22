/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_fd_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lauger <lauger@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 09:43:37 by lauger            #+#    #+#             */
/*   Updated: 2024/05/22 09:25:57 by lauger           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*read_line(char *delimiter)
{
	char	*line;

	line = readline(" > ");
	if (line == NULL)
	{
		if (line != NULL)
			free(line);
		ft_putstr_fd("bash: warning: here-document at line 1 delimited by end-of-file (wanted `%s')\n", 2);
		return (NULL);
	}
	if (ft_strcmp(line, delimiter) == 0)
	{
		if (line != NULL)
			free(line);
		return (NULL);
	}
	return (line);
}

char	*update_here_doc_content(char *line, char *here_doc_content)
{
	size_t	len;
	char	*temp;

	if (here_doc_content == NULL)
		here_doc_content = ft_strdup(line);
	else
	{
		len = ft_strlen(here_doc_content) + ft_strlen(line) + 2;
		temp = ft_calloc(len, 1);
		ft_strcpy(temp, here_doc_content);
		ft_strcat(temp, "\n");
		ft_strcat(temp, line);
		free(here_doc_content);
		here_doc_content = temp;
	}
	free(line);
	return (here_doc_content);
}

static char	*read_and_process_line(char *delimiter, char *here_doc_content)
{
	char	*line;

	line = read_line(delimiter);
	if (line == NULL)
		return (NULL);
	here_doc_content = update_here_doc_content(line, here_doc_content);
	return (here_doc_content);
}

void	handle_here_doc(t_minishell *shell, t_file here_doc, char *delimiter,
		int replace_env)
{
	char	*here_doc_content;
	char	*here_doc_content_env;
	char	*temp;

	init_signal_handlers();
	signal(SIGINT, handle_sigint_here_doc);
	here_doc_content = NULL;
	here_doc_content_env = NULL;
	while (1)
	{
		if (exit_signal == 1)
		{
			exit_signal = 0;
			free(here_doc_content);
			free_minishell(shell);
			free(here_doc.name);
			close(here_doc.fd);
			ft_printf("\n");
			exit(131);// exit(131) + recup exit code in main
		}
		temp = read_and_process_line(delimiter, here_doc_content);
		if (temp == NULL)
		{
			free(temp);
			here_doc_content = ft_strjoin(here_doc_content, "\n");
			break ;
		}
		here_doc_content = temp;
	}
	if (replace_env != 1)
	{
		here_doc_content_env = (char *)here_doc_replace_var_env(here_doc_content, shell);
		write_here_doc_in_file(here_doc_content_env, here_doc.fd, shell);
	}
	else
		write_here_doc_in_file(here_doc_content, here_doc.fd, shell);
	free_minishell(shell);
	free(here_doc.name);
	close(here_doc.fd);
	if (here_doc_content != NULL)
		free(here_doc_content);
	if (here_doc_content_env != NULL)
		free(here_doc_content_env);
	exit(EXIT_SUCCESS);
}