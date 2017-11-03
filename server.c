#include "server.h"

int main(int argc, char **argv)
{
	// int sockfd = create_server(59487);
	// accept_client(sockfd);
	// printf("%s", get_list_all_process_ids());
	printf("%s", get_thread_s_ids(1889));
	printf("\nexit\n");
	return 0;
}

int open_file(FILE **fin, const char *file_name)
{
	if (!(*fin = fopen(file_name, "r"))) {
		perror(file_name);
		return 0;
	} else {
		return 1;
	}
}

char *create_status_path(const pid_t pid)
{
	char *path = (char *) malloc(sizeof(char) * PATH_SIZE);
	snprintf(path, PATH_SIZE, "/proc/%d/status", pid);
	return path;
}

map create_status_map(FILE *fin)
{
	map status_map = NULL;
	MALLOC(status_map, sizeof(status_map));
	element_ptr curr_ptr = status_map;
	char line_buffer[BUFSIZ]; // BUFSIZ is defined in stdio.h
	int line_number = 0;
	while (fgets(line_buffer, sizeof(line_buffer), fin)) {
		// printf("%4d: %s", line_number, line_buffer);
		char *key, *value;
		split_key_value(line_buffer, &key, &value);
		if (line_number != 0) {
			MALLOC(curr_ptr -> next, sizeof(*curr_ptr));
			curr_ptr = curr_ptr -> next;
		}
		curr_ptr -> key = key;
		curr_ptr -> value = value;
		curr_ptr -> next = NULL;
		memset(line_buffer, '\0', sizeof(line_buffer));
		line_number++;
	}
	fclose(fin);
	return status_map;
}

void delete_map(const map m)
{
	element_ptr curr_ptr = m;
	while (curr_ptr != NULL) {
		element_ptr tmp = curr_ptr;
		curr_ptr = curr_ptr -> next;
		free(tmp -> key);
		free(tmp -> value);
		free(tmp);
	}
}

void split_key_value(const char *line, char **key, char **value)
{
	/*
	   Split input line into two parts: key and value
	   Example: Name:   init -> Name(key), init(value)
	   */
	*key = (char *) calloc(KEY_SIZE, sizeof(char));
	*value = (char *) calloc(VALUE_SIZE, sizeof(char));
	bool is_colon_appear = false;
	int count = 0;
	for (int i = 0; i < strlen(line); i++) {
		if (line[i] == '\n') {
			break;
		}
		if (line[i] == ':' && !is_colon_appear) {
			is_colon_appear = true;
		} else if (!is_colon_appear) {
			(*key)[i] = line[i];
		} else {
			if (line[i] == ' ' || line[i] == '\t') {
				// ignore white space and tab
				continue;
			}
			(*value)[count++] = line[i];
		}
	}
}

char* search_value(const map status_map, const char* key)
{
	element_ptr curr_ptr = status_map;
	while (curr_ptr != NULL) {
		if (strcmp(curr_ptr -> key, key) == 0) {
			return curr_ptr -> value;
		}
		curr_ptr = curr_ptr -> next;
	}
	return "ERROR: NOT_FOUND";
}

int *scan_all_digital_directories(const char *path)
{
	DIR *proc = opendir(path);
	struct dirent *entry;
	pid_t *pid_array = (pid_t *)calloc(PROC_NUM, sizeof(pid_t));
	int array_count = 0;
	pid_t pid;
	if (proc == NULL) {
		char *err_msg = (char *)calloc(ERRMSG_SIZE, sizeof(char));
		sprintf(err_msg, "opendir(%s)", path);
		perror(err_msg);
		free(err_msg);
		return NULL;
	}
	while ((entry = readdir(proc)) != NULL) {
		if (!isdigit(*entry -> d_name)) {
			continue;
		}
		pid = strtol(entry -> d_name, NULL, 10);
		pid_array[array_count++] = pid;
	}
	pid_array[array_count] = -1;
	closedir(proc);
	return pid_array;
}

int create_server(const int port)
{
	int sockfd = 0;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Fail to create a socket.");
	} else {
		printf("Server started...\n");
	}
	fflush(stdout);
	struct sockaddr_in server_info;
	memset(&server_info, 0, sizeof(server_info));
	server_info.sin_family = PF_INET;
	server_info.sin_addr.s_addr = INADDR_ANY;
	server_info.sin_port = htons(port);
	bind(sockfd, (struct sockaddr *)&server_info, sizeof(server_info));
	listen(sockfd, 5);
	return sockfd;
}

void accept_client(const int sockfd)
{
	char message[] = {"Hi,this is server.\n"};
	int client_sockfd = 0;
	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof(client_info);
	while ((1)) {
		client_sockfd = accept(sockfd, (struct sockaddr*)&client_info, &addrlen);
		printf("Accepted one\n");
		send(client_sockfd, message, sizeof(message), 0);
		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, connection_handler,
		                   (void*)&client_sockfd) < 0) {
			perror("could not create thread");
		}
		printf("Handler assigned\n");
		fflush(stdout);
	}
}

void *connection_handler(void *client_sockfd)
{
	int sockfd = *(int*)client_sockfd;
	int read_size;
	char input_buffer[BUFSIZ] = {};
	while ((read_size = recv(sockfd, input_buffer, sizeof(input_buffer), 0)) > 0 ) {
		input_buffer[read_size] = '\0';
		send(sockfd, input_buffer, sizeof(input_buffer), 0);
		printf("From %d Get: %s\n", sockfd, input_buffer);
		fflush(stdout);
		memset(input_buffer, 0, sizeof(input_buffer));
	}
	if (read_size == 0) {
		printf("Client disconnected");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}
	return 0;
}

/*char *get_process_info(char command)
  {
  switch (command) {
  case 'a':
  return get_list_all_process_ids();
  break;
  case 'b':
  return get_thread_s_ids();
  break;
  case 'c':
  return get_child_s_pids();
  break;
  case 'd':
  return get_process_name();
  break;
  case 'e':
  return get_state_of_process();
  break;
  case 'f':
  return get_cmdline();
  break;
  case 'g':
  return get_parent_s_pid();
  break;
  case 'h':
  return get_all_ancients_of_pids();
  break;
  case 'i':
  return get_virtual_memory_size();
  break;
  case 'j':
  return get_physical_memory_size();
  break;
  default:
  break;
  }
  }*/

char *get_list_all_process_ids()
{
	char *result = (char *)calloc(LIST_CHAR_LENGTH, sizeof(char));
	pid_t *pid_array = (pid_t *)scan_all_digital_directories("/proc");
	int i = 0;
	while (pid_array[i] != -1) {
		char *tmp_pid = (char *)calloc(8, sizeof(char));
		snprintf(tmp_pid, 8, (i == 0) ? "%d" : " %d", pid_array[i]);
		strcat(result, tmp_pid);
		free(tmp_pid);
		i++;
	}
	return result;
}

char *get_thread_s_ids(pid_t pid)
{
	char *pid_task_path = (char *) malloc(sizeof(char) * PATH_SIZE);
	snprintf(pid_task_path, PATH_SIZE, "/proc/%d/task", pid);
	char *result = (char *)calloc(LIST_CHAR_LENGTH, sizeof(char));
	tid_t *tid_array = (pid_t *)scan_all_digital_directories(pid_task_path);
	int i = 0;
	while (tid_array[i] != -1) {
		char *tmp_pid = (char *)calloc(8, sizeof(char));
		snprintf(tmp_pid, 8, (i == 0) ? "%d" : " %d", tid_array[i]);
		strcat(result, tmp_pid);
		free(tmp_pid);
		i++;
	}
	return result;
}
/*
   char *get_child_s_pids(){}
   char *get_process_name(){}
   char *get_state_of_process(){}
   char *get_cmdline(){}
   char *get_parent_s_pid(){}
   char *get_all_ancients_of_pids(){}
   char *get_virtual_memory_size(){}
   char *get_physical_memory_size(){}*/
