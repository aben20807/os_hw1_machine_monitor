#include "server.h"

int main(int argc, char **argv)
{
	int sockfd = create_server(59487);
	accept_client(sockfd);
	// printf("a) %s\n", get_process_info('a', 0));
	// printf("b) %s\n", get_process_info('b', 1));
	// printf("c) %s\n", get_process_info('c', 1));
	// printf("d) %s\n", get_process_info('d', 5));
	// printf("e) %s\n", get_process_info('e', 1));
	// printf("f) %s\n", get_process_info('f', 1));
	// printf("g) %s\n", get_process_info('g', 1));
	// printf("h) %s\n", get_process_info('h', 5));
	// printf("i) %s\n", get_process_info('i', 1));
	// printf("j) %s\n", get_process_info('j', 1));
	// while (1) {
	//     printf("which? ");
	//     char command;
	//     scanf(" %c", &command);
	//     if (command == 'k') {
	//         break;
	//     }
	//     pid_t pid = 0;
	//     if (command != 'a') {
	//         printf("pid? ");
	//         scanf("%d", &pid);
	//     }
	//     printf(">  %s\n", get_process_info(command, pid));
	// }
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
	char *path = NULL;
	MALLOC(path, sizeof(char) * PATH_SIZE);
	snprintf(path, PATH_SIZE, "/proc/%d/status", pid);
	return path;
}

char *create_cmdline_path(const pid_t pid)
{
	char *path = NULL;
	MALLOC(path, sizeof(char) * PATH_SIZE);
	snprintf(path, PATH_SIZE, "/proc/%d/cmdline", pid);
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
	CALLOC(*key, KEY_SIZE, sizeof(char));
	CALLOC(*value, VALUE_SIZE, sizeof(char));
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
			if (strcmp(*key, "State") == 0 && count == 1) {
				// store only first char if key is "State"
				break;
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
	return "ERROR: FIELD_NOT_FOUND";
}

int *scan_all_digital_directories(const char *path)
{
	DIR *dir = opendir(path);
	struct dirent *entry;
	pid_t *pid_array = NULL;
	CALLOC(pid_array, PROC_NUM, sizeof(pid_t));
	int array_count = 0;
	pid_t pid;
	if (dir == NULL) {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		sprintf(err_msg, "opendir(%s)", path);
		perror(err_msg);
		free(err_msg);
		return NULL;
	}
	while ((entry = readdir(dir)) != NULL) {
		if (!isdigit(*entry -> d_name)) {
			continue;
		}
		pid = strtol(entry -> d_name, NULL, 10);
		pid_array[array_count++] = pid;
	}
	pid_array[array_count] = -1;
	closedir(dir);
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
	int client_sockfd = 0;
	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof(client_info);
	while ((1)) {
		client_sockfd = accept(sockfd, (struct sockaddr*)&client_info, &addrlen);
		printf("Accepted one\n");
		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, connection_handler,
		                   (void *)&client_sockfd) < 0) {
			perror("could not create thread");
		}
		printf("Handler assigned to thread %li\n", thread_id);
		fflush(stdout);
	}
}

void *connection_handler(void *client_sockfd)
{
	int sockfd = *(int*)client_sockfd;
	int read_size;
	struct monitor_protocol package;
	while ((read_size = recv(sockfd, &package, sizeof(package), 0)) > 0 ) {
		printf("From %d Get: %c %d\n", sockfd, package.command, package.pid);
		char *desc = get_process_description(package.command);
		char *info = get_process_info(package.command, package.pid);
		strncpy(package.description, desc, DESCRIPTION_SIZE);
		strncpy(package.info, info, BUFSIZ);
		send(sockfd, &package, sizeof(package), 0);
		fflush(stdout);
		free(info);
	}
	if (read_size == 0) {
		printf("Client disconnected\n");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}
	return 0;
}

char *convert_int_array_to_char_array(int *int_array)
{
	char *result = NULL;
	CALLOC(result, LIST_CHAR_LENGTH, sizeof(char));
	int i = 0;
	while (int_array[i] != -1) {
		char *tmp_pid = NULL;
		CALLOC(tmp_pid, ID_WIDTH, sizeof(char));
		snprintf(tmp_pid, ID_WIDTH, (i == 0) ? "%d" : " %d", int_array[i]);
		strcat(result, tmp_pid);
		free(tmp_pid);
		i++;
	}
	free(int_array);
	return result;
}

char *get_status_file_field(const pid_t pid, const char *field)
{
	FILE *fin;
	if (!open_file(&fin, create_status_path(pid))) {
		return "ERROR: FILE_NOT_FOUND";
	}
	map status_map = create_status_map(fin);
	char *result = NULL;
	CALLOC(result, VALUE_SIZE, sizeof(char));
	strncpy(result, search_value(status_map, field), VALUE_SIZE);
	delete_map(status_map);
	return result;
}

char *get_process_info(const char command, const pid_t pid)
{
	switch (command) {
	case 'a':
		return get_list_all_process_ids();
		break;
	case 'b':
		return get_thread_s_ids(pid);
		break;
	case 'c':
		return get_child_s_pids(pid);
		break;
	case 'd':
		return get_status_file_field(pid, "Name");
		break;
	case 'e':
		return get_status_file_field(pid, "State");
		break;
	case 'f':
		return get_cmdline(pid);
		break;
	case 'g':
		return get_status_file_field(pid, "PPid");
		break;
	case 'h':
		return get_all_ancients_of_pids(pid);
		break;
	case 'i':
		return get_status_file_field(pid, "VmSize");
		break;
	case 'j':
		return get_status_file_field(pid, "VmRSS");
		break;
	default:
		return "ERROR: COMMAND_NOT_FOUND";
		break;
	}
}

char *get_process_description(const char command)
{
	switch (command) {
	case 'a':
		return "[all processes ids]";
		break;
	case 'b':
		return "[tid]";
		break;
	case 'c':
		return "[children pids]";
		break;
	case 'd':
		return "[process name]";
		break;
	case 'e':
		return "[process state]";
		break;
	case 'f':
		return "[command line]";
		break;
	case 'g':
		return "[parent's pid]";
		break;
	case 'h':
		return "[ancients' pids]";
		break;
	case 'i':
		return "[virtual memory]";
		break;
	case 'j':
		return "[physical memory]";
		break;
	default:
		return "";
		break;
	}
}

char *get_list_all_process_ids()
{
	pid_t *pid_array = (pid_t *)scan_all_digital_directories("/proc");
	if (pid_array == NULL) {
		return "ERROR: FILE_NOT_FOUND";
	}
	return convert_int_array_to_char_array((int *)pid_array);
}

char *get_thread_s_ids(const pid_t pid)
{
	char *pid_task_path = NULL;
	MALLOC(pid_task_path, sizeof(char) * PATH_SIZE);
	snprintf(pid_task_path, PATH_SIZE, "/proc/%d/task", pid);
	tid_t *tid_array = (pid_t *)scan_all_digital_directories(pid_task_path);
	if (tid_array == NULL) {
		return "ERROR: FILE_NOT_FOUND";
	}
	free(pid_task_path);
	return convert_int_array_to_char_array((int *)tid_array);
}

char *get_child_s_pids(const pid_t pid)
{
	pid_t *pid_array = (pid_t *)scan_all_digital_directories("/proc");
	if (pid_array == NULL) {
		return "ERROR: FILE_NOT_FOUND";
	}
	pid_t *children_list = NULL;
	CALLOC(children_list, PROC_NUM, sizeof(pid_t));
	char *tmp_child = NULL;
	CALLOC(tmp_child, ID_WIDTH, sizeof(char));
	int pid_count = 0, child_count = 0;
	snprintf(tmp_child, ID_WIDTH, "%d", pid);
	while (pid_array[pid_count] != -1) {
		if (strcmp(tmp_child, get_status_file_field(pid_array[pid_count],
		           "PPid")) == 0) {
			children_list[child_count++] = pid_array[pid_count];
		}
		pid_count++;
	}
	children_list[child_count] = -1;
	free(tmp_child);
	free(pid_array);
	return convert_int_array_to_char_array(children_list);
}

char *get_cmdline(const pid_t pid)
{
	FILE *fin;
	if (!open_file(&fin, create_cmdline_path(pid))) {
		return "ERROR: FILE_NOT_FOUND";
	}
	char *result = NULL;
	CALLOC(result, CMDLINE_SIZE, sizeof(char));
	char c;
	int count = 0;
	while ((c = fgetc(fin)) != EOF) { //read all char until end of file
		result[count++] = c;
	}
	fclose(fin);
	if (strlen(result) == 0) {
		return "(nothing in the cmdline)";
	} else {
		return result;
	}
}

char *get_all_ancients_of_pids(const pid_t pid)
{
	pid_t tmp_pid = pid;
	char *result = NULL;
	CALLOC(result, LIST_CHAR_LENGTH, sizeof(char));
	char *tmp_ppid;
	int i = 0;
	while (strcmp("0", tmp_ppid = get_status_file_field(tmp_pid, "PPid")) != 0) {
		if (strcmp("ERROR: FILE_NOT_FOUND", tmp_ppid) == 0 ) {
			return "ERROR: FILE_NOT_FOUND";
		}
		if (i > 0) {
			strcat(result, " ");
		}
		strcat(result, tmp_ppid);
		tmp_pid = atoi(tmp_ppid);
		free(tmp_ppid);
		i++;
	}
	strcat(result, (strlen(result) == 0) ? "0" : " 0"); // add 0 ppid
	return result;
}
