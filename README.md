## About
Reggatad --- is a service (daemon process) that makes it possible to add/remove/modify `tags` to regular files and search by tags. Beside the `tags` there are `fields` (key-value pairs) that are also searchable. Reggatad also watches for changes in filesystem and updates tags database correspondingly. It stores all tags/fields information in database and provides an API (TCP sockets) for all operations with tags/fields. This API uses reggata_client (https://github.com/av-elier/reggata-scala-client). This project is a second try of https://github.com/vlkv/reggata. At the moment, reggatad is a **work in progress**.

It uses
- Flexc++ and Bisonc++ to implement query language parsing
- RocksDB http://rocksdb.org/docs/getting-started.html an embedded database (key value store)
- POCO DirectoryWatcher class (implemented with inotify on Linux)
- https://github.com/nlohmann/json as JSON library
- gtest as unit testing framework
- boost for anything else

## API
Every message has a **4 byte header** that contains length of the message. The message is a JSON string.
Every message has an 'id' member. This 'id' is always inserted in the response.
All commands are divided into two categories:
* operations with the repos as a whole (open_repo/close_repo), search across the all opened repos 
(search without 'path' param), cancellation of another command (cancel_cmd).
* operations with files withing one repo (add_tags/remove_tags/get_file_info/search with 'path').


### open_repo(root_dir, db_dir, init_if_not_exists)
Request:
```json
{
	id: "1",
	cmd: "open_repo",
	args: {
		root_dir: "/home/repo/",
		db_dir: "home/repo/.reggata/",
		init_if_not_exists: true
	}
}
```
Responses:
```json
{ok: true, id: "1"}
{ok: false, msg: "Reason", id: "1"}
```	
### close_repo(root_dir)
Request:
```json
{
	id: "1",
	cmd: "close_repo",
	args: {
		root_dir: "/home/repo/"
	}
}
```

### get_repos_info()
Request:
```json
{
	id: "1",
	cmd: "get_repos_info"
}
```
Response:
```json
{
	ok: true,
	repos: [
		{root_dir:"/home/repo1/"},
		{root_dir:"/home/repo2/"}
	]
}
```
	
### add_tags(file_path, tag1, tag2, ...)
Request:
```json
{
	id: "1",
	cmd: "add_tags",
	args: {
		file: "/home/repo/file.txt",
		tags: ["tag1", "tag2"]
	}
}
```
### remove_tags(file_path, tag1, tag2, ...)
Request:
```json
{
	id: "1",
	cmd: "remove_tags",
	args: {
		file: "/home/repo/file.txt",
		tags: ["tag1", "tag2"]
	}
}
```
TODO: do we need a request to remove all tags from file?..
TODO: do we need a request to get info about "tag cloud"? It's a N most popular tags with their usage counts.

### add_fields(file_path, field1(key1,val1), field2(key2,val2), ...)
TODO: implement after tags

### remove_fields(file_path, field_key1, field_key2, ...)
TODO: implement after tags

### get_file_info(file_path)
Request:
```json
{
	id: "1",
	cmd: "get_file_info",
	args: {
		file: "/home/repo/file.txt"
	}
}
```

### search(path, query_string)
Request:
```json
{
	id: "1",
	cmd: "search",
	args: {
		path: "/home/repo/",
		query: "tag1 tag2|tag3"
	}
}
```

### cancel_cmd(cmd_id)
Client is able to cancel any other long running command.
Request:
```json
{
	id: "2",
	cmd: "cancel_cmd",
	args: {
		cmd_id: "1"
	}
}
```

## File watch actions
### file_created - do nothing
### file_removed - remove file tags and fields from DB
### file_moved (renamed) - update file path
### file_modified - do nothing
### dir_created - create a filewatch for it
### dir_removed - remove all files tags and fields from DB recursively. Remove filewathes from dir and subdirs
### dir_moved (renamed) - update files paths for all files recursively. Remove/Create a filewatcher for the dir

## Query Language
Fields are just tags with values. Values should be typed. 
Types supported are: string, number, datetime.
Operations with tags are: AND (lowest priority), OR, NOT and braces (highest priority).
Operations with fields are: ==, !=, >, >=, <, <=, ~= (like)

files which has all three tags: t1 AND t2 AND t3:
```
t1 t2 t3
```
files with tag t1 AND any of t2 OR t3:
```
t1 t2|t3
```
files with tags t1 AND t2 OR just one tag t3:
```
(t1 t2)|t3
``` 
files with tag t1 AND field f1>5:
```
t1 f1>5
``` 
files with tag t1 OR field f1>5:
```
t1 | f1>5
``` 

"tag1" eqiuvalent to "tag1 != NULL" => all files with tag1
"NOT tag1" equivalent "tag1 == NULL" => all files without tag1

The obvious way of executing queries is just filter files by subdir (recursively), then iterate over them and 
apply query predicate to every file. Very often case is to perform a query in a subdir. The mechanism for executing 
query in the repo root is just a particular case of "exec query in subdir".

The old grammar of reggata is https://github.com/vlkv/reggata/wiki/Query-language-grammar. In reggatad we should improve it.

## Some NOTEs and thoughts
* Repository should have a list of required fileds. Every file with tags should have these fields set. E.g. 'rating'.
* Reggatad should have some recovery request in API. For example, by some reason reggatad wasn't running. And during that time user moved (or anything) files withing repository... This recovery could be a simple one. For example we try to find files in different location withing the repo by name...
* Maybe we should also provide add/remove tags/fields to all files in subdir recursively. Or this would be a task for reggata_client?..

## Useful links
* https://github.com/eliben/code-for-blog/blob/master/2011/asio_protobuf_sample/db_server.cpp
