數字意義
-----
紅色棋為敵方，藍色棋為己方<br>
數字0-17為步兵<br>
數字18、19為角行<br>
數字20、21為飛車<br>
數字22-25為香車<br>
數字26-29為桂馬<br>
數字30-33為銀將<br>
數字34-37為金將<br>
數字38、39為王將<br>

Load last game
-----
compile後首先要選擇是否回放上一盤棋局，1 for yes, 0 for no  <br>
如果要回放，輸入1可以看下一步，0可以看上一步<br>
如果選擇不撥放則會進入下棋階段<br>
Problem:<br>
由於寫檔時未判斷棋子所屬陣營，因此在load last game時，棋子的顏色會出現錯誤<br>

下棋
-------
首先輸入欲移動的棋所在位置的Row、Column<br>
如果要毀棋請在ROW輸入100<br>
接著輸入目標位置的Row、Column<br>

當輸入有誤會出現以下種輸出，需重新輸入欲移動的棋子位置以及目標位置
-------
輸出 IT'S NOT YOUR TURN YET!，表示欲移動的棋子不屬於現在的下棋方<br>
輸出 YOU CANNOT WALK THIS WAY!，表示移動路徑有誤(不符合該棋的移動方式)<br>
輸出 THERE ARE NO CHESSES HERE!表示欲移動的位置裡面沒有棋<br>
輸出 YOU CANNOT PLACE YOUR CHESS HERE!，表示目標位置上有同陣營的棋子<br>

輸入合法
-------
會顯示移動後的棋盤現況<br>

升級
-------
當紅色棋進入第7-9行時，可以選擇是否升級<br>
當藍色棋進入第1-3行時，可以選擇是否升級<br>
升級後的棋會有不同移動方式<br>

吃棋
-------
Red chessbox會顯示紅方吃掉的棋子<br>
Blue chessbox會顯示藍方吃掉的棋子<br>
只要chessbox裡面有棋子，之後的每次下棋前系統會詢問是否要從chessbox放棋，1 for yes 0 for no<br>
可以按chessbox裡面標示的位置選擇要放的棋，例如該棋放在第0格，則輸入0以放置該棋<br>

遊戲結束
------
當任一方的王將被吃，遊戲結束，棋局會存入Previous game.txt<br>
若欲回放棋局，需重新compile

