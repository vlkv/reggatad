## About
**Reggatad** --- is a service (daemon process) that makes it possible to add/remove/modify 
*tags* to regular files and search by those tags. Beside the tags there are *fields* 
(key-value pairs) that are also searchable. Reggatad also watches for changes in filesystem 
and updates tags database correspondingly. It stores all tags/fields information in database 
and provides an API (TCP sockets) for all operations with tags/fields. This API uses reggata_client 
(https://github.com/av-elier/reggata-scala-client). This project is a second try of 
https://github.com/vlkv/reggata. At the moment, reggatad is a **work in progress**.

It uses
- Flexc++ and Bisonc++ to implement query language parsing https://fbb-git.github.io/flexcpp/ https://github.com/fbb-git/bisoncpp
- RocksDB http://rocksdb.org an embedded database (key value store)
- POCO DirectoryWatcher class (implemented with inotify on Linux) https://pocoproject.org/
- JSON for Modern C++ https://github.com/nlohmann/json
- GTest as unit testing framework https://github.com/google/googletest
- Boost for anything else http://www.boost.org/

## API
Every message has a **4 byte header** that contains length of the message. The message is a JSON string.
Every message has an 'id' member. This 'id' is always inserted in the response.
All commands are divided into two categories:
* operations with the repos as a whole (open_repo/close_repo), search across the all opened repos 
(search without 'path' param), cancellation of another command (cancel_cmd).
* operations with files withing one repo (add_tags/remove_tags/get_file_info/search with 'path').


### open_repo(root_dir, db_dir, init_if_not_exists)
Request:
```javascript
{
    id: "1",
    cmd: "open_repo",
    args: {
            root_dir: "/home/repo/",
            db_dir: "/home/repo/.reggata/",
            init_if_not_exists: true
    }
}
```
TODO: At the moment `db_dir` is absolute path. Let client to use relative path 
for `db_dir`. Make argument `db_dir` optional.

Responses:
```javascript
{code: 200, id: "1"}
{code: 400, msg: "Reason", id: "1"}
```
NOTE: list of all possible code values is here https://github.com/vlkv/reggatad/blob/master/src/status_code.cpp

### TODO: close_repo(root_dir)
Request:
```javascript
{
    id: "1",
    cmd: "close_repo",
    args: {
        root_dir: "/home/repo/"
    }
}
```

### TODO: get_repos_info()
Request:
```javascript
{
    id: "1",
    cmd: "get_repos_info"
}
```
Response:
```javascript
{
    code: 200,
    id: "1",
    data: [
        {root_dir:"/home/repo1/"},
        {root_dir:"/home/repo2/"}
    ]
}
```
	
### add_tags(file_path, tag1, tag2, ...)
Request:
```javascript
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
```javascript
{
    id: "1",
    cmd: "remove_tags",
    args: {
        file: "/home/repo/file.txt",
        tags: ["tag1", "tag2"]
    }
}
```

TODO: We need a request to remove all tags/fields from file

TODO: do we need a request to get info about "tag cloud"? It's a N most popular tags with their usage counts.

### TODO: add_fields(file_path, field1(key1,val1), field2(key2,val2), ...)

### TODO: remove_fields(file_path, field_key1, field_key2, ...)

### get_file_info(file_path)
Request:
```javascript
{
    id: "1",
    cmd: "get_file_info",
    args: {
        file: "/home/user/repo/dir/file"
    }
}
```
Response:
```javascript
{
    code: 200,
    id: "1",
    data: {
        "path": "dir/file",
        "size": 4,
        "tags": ["tag1", "tag2", "tag3"]
    }
}
```

### TODO: get_dir_files_info(dir_path)
Request:
```javascript
{
    id: "1",
    cmd: "get_dir_files_info",
    args: {
        dir: "/home/user/repo/dir/",
        recursive: false
    }
}
```
Response:
```javascript
{
    code: 200,
    id: "1",
    data: [{
            "path": "dir/file1",
            "size": 4,
            "tags": ["tag1", "tag2", "tag3"]
        }, {
            "path": "dir/file2",
            "size": 1024,
            "tags": ["ABC", "DEF"]
        }
    ]
}
```

### TODO: search(dir_abs_path, query_string)
Request:
```javascript
{
    id: "1",
    cmd: "search",
    args: {
        dir: "/home/repo/",
        query: "tag1 tag2|tag3"
    }
}
```

Response is the same as `get_dir_files_info` response (collection of file_infos).

### TODO: cancel_cmd(cmd_id)
Client is able to cancel any other long running command.
Request:
```javascript
{
    id: "2",
    cmd: "cancel_cmd",
    args: {
        cmd_id: "1"
    }
}
```

## File watch actions
* file_created - do nothing
* file_removed - remove file tags and fields from DB
* file_moved (renamed) - update file path
* file_modified - do nothing
* dir_created - create a filewatch for it
* dir_removed - remove all files tags and fields from DB recursively. Remove filewathes from dir and subdirs
* dir_moved (renamed) - update files paths for all files recursively. Remove/Create a filewatcher for the dir

## Search Query Language
Operations with tags are: `&` or simply space (AND, conjunction, lowest priority), `|` (OR, disjunction), 
`!` (NOT, negation), operations with fields (`==`, `!=`, `>`, `>=`, `<`, `<=`, `~=` (like)) 
and braces `(`, `)` (highest priority).

Fields are just tags with values. Values should be typed. 
Types supported are: string, number, datetime.

Files which has all three tags: t1 AND t2 AND t3:
`t1 t2 t3` equivalent to `t1 & t2 & t3`.

Files with tag t1 AND any of t2 OR t3:
`t1 t2 | t3` it's the same as `t1 (t2 | t3)` because `|` has higher priority than `&`.

Files with tags t1 AND t2 OR just one tag t3:
`(t1 t2) | t3`.

Files with tag t1 AND field f1>5:
`t1 f1>5`.

Files with tag t1 OR field f1>5:
`t1 | f1>5`.

Files that doesn't have tag t1:
`NOT t1`.

The obvious way of executing queries is just filter files by subdir (recursively), then iterate over them and 
apply query predicate to every file. Very often case is to perform a query in a subdir. The mechanism for executing 
query in the repo root is just a particular case of "exec query in subdir".

Another way is to calculate separate sets in memory of file_ids for every tag and field. Then
calcuate AND/OR/NOT/etc operations on them according to the query structure.

Lexer grammar: https://github.com/vlkv/reggatad/blob/master/src/scanner/scanner.g

Parser grammar: https://github.com/vlkv/reggatad/blob/master/src/parser/parser.g


## Some NOTEs and thoughts
* Repository should have a list of required fileds. Every file with tags should have these fields set. E.g. 'rating'.
* Reggatad should have some recovery request in API. For example, by some reason reggatad wasn't running. And during that time user moved (or anything) files withing repository... This recovery could be a simple one. For example we try to find files in different location withing the repo by name...
* Maybe we should also provide add/remove tags/fields to all files in subdir recursively. Or this would be a task for reggata_client?..
* Alternatives for RocksDB are
  * http://www.sqlite.org/
  * https://upscaledb.com/
  * http://www.wiredtiger.com/
  * http://unqlite.org/
  * more http://embedded-databases.com/

## RocksDB Database Schema

API is here https://github.com/facebook/rocksdb/blob/v4.9/include/rocksdb/db.h

Example of data in form of (column_family, key, value).
```

(file_path, /a/b, 1) // This is like an index on "path" property
(file, 1:path, /a/b)
(file, 1:size, 1234)
(file, 1:prop, value)

(file_path, /a/c, 2)
(file, 2:path, /a/c)
(file, 2:size, 234234)

(file_path, /a/d, 3)
(file, 3:path, /a/d)
(file, 3:size, 63435)

(file_tag, 1:Tag1, "")
(tag_file, Tag1:1, "")

(file_tag, 1:Tag2, "")
(tag_file, Tag2:1, "")

(file_tag, 2:Tag1, "")
(tag_file, Tag1:2, "")

(file_field, 1:Field1, "10")
(field_file, Field1:1, "")

(file_field, 2:Field1, "20")
(field_file, Field1:2, "")

(file_field, 2:Field2, "21")
(field_file, Field2:2, "")
```

When a tag is attached to a file, a pair of key-values (e.g. file_tag and tag_file) should be added atomically with
WriteBatch operation in rocksDB.

## Flex and Bison C++

From https://fbb-git.github.io/flexcpp/manhtml/flexc++.1.html 
To generate lexer, execute:
```
$ flexc++ scanner.g
```

This would generate 4 files:
```
lex.cc  Scannerbase.h  Scanner.h  Scanner.ih
```

To generate parser, execute:
```
$ bisonc++ parser.g
```

This would generate 4 files:
```
parse.cc  Parserbase.h  Parser.h  Parser.ih
```


