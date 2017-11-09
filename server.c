#include "server.h"

int main(int argc, char **argv)
{
	int sockfd = create_server(59487);
	accept_client(sockfd);
	printf("\nexit\n");
	return 0;
}

static inline int open_file(FILE **fin, const char *file_name)
{
	if (!(*fin = fopen(file_name, "r"))) {
		perror(file_name);
		return 0;
	} else {
		return 1;
	}
}

static inline char *create_status_path(const pid_t pid)
{
	char *path = NULL;
	MALLOC(path, sizeof(char) * PATH_SIZE);
	snprintf(path, PATH_SIZE, "/proc/%d/status", pid);
	return path;
}

static inline char *create_cmdline_path(const pid_t pid)
{
	char *path = NULL;
	MALLOC(path, sizeof(char) * PATH_SIZE);
	snprintf(path, PATH_SIZE, "/proc/%d/cmdline", pid);
	return path;
}

static inline map create_status_map(FILE *fin)
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

static inline void delete_map(const map m)
{
	element_ptr curr_ptr = m;
	while (curr_ptr != NULL) {
		element_ptr tmp = curr_ptr;
		curr_ptr = curr_ptr -> next;
		FREE(tmp -> key);
		FREE(tmp -> value);
		FREE(tmp);
	}
}

/*
 * Split input line into two parts: key and value for map elements
 * Example: "Name:   init" -> "Name"(key), "init"(value)
 */
static inline void split_key_value(const char *line, char **key, char **value)
{
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

static inline char* search_value(const map status_map, const char* key)
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

static inline int *scan_all_digital_dir(const char *path)
{
	DIR *dir = opendir(path);
	if (dir == NULL) {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		sprintf(err_msg, "opendir(%s)", path);
		perror(err_msg);
		FREE(err_msg);
		return NULL;
	}
	struct dirent *entry;
	pid_t *pid_list = NULL;
	CALLOC(pid_list, PROC_NUM, sizeof(pid_t));
	pid_t pid;
	int count = 0;
	while ((entry = readdir(dir)) != NULL) {
		if (!isdigit(*entry -> d_name)) {
			continue;
		}
		pid = strtol(entry -> d_name, NULL, ID_WIDTH);
		pid_list[count++] = pid;
	}
	pid_list[count] = -1;
	closedir(dir);
	return pid_list;
}

static inline int create_server(const int port)
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
	listen(sockfd, MAX_LISTEN_NUM);
	return sockfd;
}

static inline void accept_client(const int sockfd)
{
	int client_sockfd = 0;
	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof(client_info);
	while (true) {
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

static void *connection_handler(void *client_sockfd)
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
		FREE(info);
	}
	if (read_size == 0) {
		printf("Client disconnected\n");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}
	return 0;
}

static inline char *convert_int_array_to_string(int *int_array)
{
	char *result = NULL;
	CALLOC(result, LIST_CHAR_LENGTH, sizeof(char));
	int i = 0;
	while (int_array[i] != -1) {
		char *tmp_pid = NULL;
		CALLOC(tmp_pid, ID_WIDTH, sizeof(char));
		snprintf(tmp_pid, ID_WIDTH, (i == 0) ? "%d" : " %d", int_array[i]);
		strcat(result, tmp_pid);
		FREE(tmp_pid);
		i++;
	}
	FREE(int_array);
	return result;
}

static char *get_status_file_field(const pid_t pid, const char *field)
{
	FILE *fin = NULL;
	char *path = create_status_path(pid);
	if (!open_file(&fin, path)) {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		strncpy(err_msg, "ERROR: FILE_NOT_FOUND", ERRMSG_SIZE);
		FREE(path);
		return err_msg;
	}
	char *result = NULL;
	CALLOC(result, VALUE_SIZE, sizeof(char));
	map status_map = create_status_map(fin);
	strncpy(result, search_value(status_map, field), VALUE_SIZE);
	delete_map(status_map);
	FREE(path);
	return result;
}

static inline char *get_process_info(const char command, const pid_t pid)
{
	switch (command) {
	case 'a':
		return get_list_all_process_ids();
	case 'b':
		return get_thread_s_ids(pid);
	case 'c':
		return get_child_s_pids(pid);
	case 'd':
		return get_status_file_field(pid, "Name");
	case 'e':
		return get_status_file_field(pid, "State");
	case 'f':
		return get_cmdline(pid);
	case 'g':
		return get_status_file_field(pid, "PPid");
	case 'h':
		return get_all_ancestors_of_pids(pid);
	case 'i':
		return get_status_file_field(pid, "VmSize");
	case 'j':
		return get_status_file_field(pid, "VmRSS");
	default: {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		strncpy(err_msg, "ERROR: COMMAND_NOT_FOUND", ERRMSG_SIZE);
		return err_msg;
	}
	}
}

static inline char *get_process_description(const char command)
{
	switch (command) {
	case 'a':
		return "[all processes ids]";
	case 'b':
		return "[tid]";
	case 'c':
		return "[children pids]";
	case 'd':
		return "[process name]";
	case 'e':
		return "[process state]";
	case 'f':
		return "[command line]";
	case 'g':
		return "[parent's pid]";
	case 'h':
		return "[ancestors' pids]";
	case 'i':
		return "[virtual memory]";
	case 'j':
		return "[physical memory]";
	default:
		return "";
	}
}

static inline char *get_list_all_process_ids()
{
	pid_t *pid_list = (pid_t *)scan_all_digital_dir("/proc");
	if (pid_list == NULL) {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		strncpy(err_msg, "ERROR: FILE_NOT_FOUND", ERRMSG_SIZE);
		return err_msg;
	}
	return convert_int_array_to_string((int *)pid_list);
}

static inline char *get_thread_s_ids(const pid_t pid)
{
	char *pid_task_path = NULL;
	MALLOC(pid_task_path, sizeof(char) * PATH_SIZE);
	snprintf(pid_task_path, PATH_SIZE, "/proc/%d/task", pid);
	tid_t *tid_list = (pid_t *)scan_all_digital_dir(pid_task_path);
	if (tid_list == NULL) {
		char *err_msg = NULL;
		CALLOC(err_msg, CMDLINE_SIZE, sizeof(char));
		strncpy(err_msg, "ERROR: FILE_NOT_FOUND", ERRMSG_SIZE);
		FREE(tid_list);
		FREE(pid_task_path);
		return err_msg;
	}
	FREE(pid_task_path);
	return convert_int_array_to_string((int *)tid_list);
}

static inline char *get_child_s_pids(const pid_t pid)
{
	pid_t *pid_list = (pid_t *)scan_all_digital_dir("/proc");
	if (pid_list == NULL) {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		strncpy(err_msg, "ERROR: FILE_NOT_FOUND", ERRMSG_SIZE);
		return err_msg;
	}
	pid_t *children_list = NULL;
	CALLOC(children_list, PROC_NUM, sizeof(pid_t));
	char *target_ppid = NULL;
	CALLOC(target_ppid, ID_WIDTH, sizeof(char));
	snprintf(target_ppid, ID_WIDTH, "%d", pid);
	int pid_count = 0, child_count = 0;
	while (pid_list[pid_count] != -1) {
		char *tmp_ppid = get_status_file_field(pid_list[pid_count], "PPid");
		if (strcmp(target_ppid, tmp_ppid) == 0) {
			children_list[child_count++] = pid_list[pid_count];
		}
		FREE(tmp_ppid);
		pid_count++;
	}
	children_list[child_count] = -1;
	FREE(target_ppid);
	FREE(pid_list);
	return convert_int_array_to_string(children_list);
}

static inline char *get_cmdline(const pid_t pid)
{
	FILE *fin = NULL;
	char *path = create_cmdline_path(pid);
	if (!open_file(&fin, path)) {
		char *err_msg = NULL;
		CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
		strncpy(err_msg, "ERROR: FILE_NOT_FOUND", ERRMSG_SIZE);
		FREE(path);
		return err_msg;
	}
	char c;
	int count = 0;
	char *result = NULL;
	CALLOC(result, CMDLINE_SIZE, sizeof(char));
	while ((c = fgetc(fin)) != EOF) { //read all char until end of file
		result[count++] = c;
	}
	fclose(fin);
	FREE(path);
	if (strlen(result) == 0) {
		strncpy(result, "(nothing in the cmdline)", CMDLINE_SIZE);
		return result;
	} else {
		return result;
	}
}

static inline char *get_all_ancestors_of_pids(const pid_t pid)
{
	pid_t tmp_pid = pid;
	char *result = NULL;
	CALLOC(result, LIST_CHAR_LENGTH, sizeof(char));
	char *tmp_ppid;
	int i = 0;
	while (strcmp("0", tmp_ppid = get_status_file_field(tmp_pid, "PPid")) != 0) {
		if (strcmp("ERROR: FILE_NOT_FOUND", tmp_ppid) == 0 ) {
			char *err_msg = NULL;
			CALLOC(err_msg, ERRMSG_SIZE, sizeof(char));
			strncpy(err_msg, "ERROR: FILE_NOT_FOUND", ERRMSG_SIZE);
			FREE(tmp_ppid);
			FREE(result);
			return err_msg;
		}
		if (i > 0) {
			strcat(result, " ");
		}
		strcat(result, tmp_ppid);
		tmp_pid = atoi(tmp_ppid);
		FREE(tmp_ppid);
		i++;
	}
	strcat(result, (strlen(result) == 0) ? "0" : " 0"); // add 0 ppid
	return result;
}
