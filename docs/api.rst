.. c:function:: int UPDATE_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
    update exe main function, use to input is 'update' and parse.
    main completely xsql instruction is : 
    [XSQL] 'update tableName(field1 , field2 , ..) set ('value1' , 'value2' , ..) where field1 = 'searchF1' and field2 = 'search2' and/or ..;'
            ^^^^^^                                 ^^^                            ^^^^^                     ^^^
.. c:function:: int SHOW_exe(TABLE_LIST_NODE *head,TOKENSB *tokensb , int curr);
    show exe main function, use to input is 'show' and parse.
    main tackle : [XSQL] 'show tables;' , [XSQL] 'show table tableName;' , [XSQL] 'show table dataline;' so on instruction.
                        ^^^^                    ^^^^                             ^^^^
.. c:function:: 