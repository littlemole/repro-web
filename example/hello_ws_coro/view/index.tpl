<!DOCTYPE html>
<html>
<head>
<title>Hello Coro Websockets</title>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link href="https://fonts.googleapis.com/css?family=Noto+Sans|Roboto+Mono|Titillium+Web" rel="stylesheet"> 
<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.2.0/css/all.css" integrity="sha384-hWVjflwFxL6sNzntih27bfxkr27PmbbK/iSvJ+a4+0owXq79v+lsFkW54bOGbiDQ" crossorigin="anonymous">
<link rel="stylesheet" href="css/style.css">
<style>
</style>
</head>
<body>

<!-- Header -->
<div class="header">
  <b>repro-web</b>
  <span>modern reactive c++ for the web.</span>
  <a href="https://github.com/littlemole/repro-web">repro-web home</a>
  <a href="https://github.com/littlemole/repro-web/tree/master/example">examples</a>
  <a href="https://github.com/littlemole/repro-web/tree/master/example/hello_ws_coro">this example</a>
</div>

<div class="frame">
<div class="title">
    <h1>hello coroutine websockets</h1>
    <p>hello world websocket example with coroutines for modern reactive c++ for the web.</p>
</div>
</div>



<!-- The flexible grid (content) -->
<div class="row">
  <div class="side">
      <h2>Hello WSCORO Menu</h2>
  
<!-- Navigation Bar -->
     <div class="navbar">

      <ul class="nav">
       <li>
            <a href="/">home</a>
       </li>
       <li>
            <a href="/login">login</a>
       </li>
       <li>
            <a href="/register">register</a>
        </li>
        <li>
                <a href="/logout">logout</a>
            </li>
        </ul>
    
    </div>      
  </div>
  <div id="sitedivider"></div>
  <div class="main">
      <h2>Hello Coroutines WebSockets Example</h2>

      <h5>Hello dear {{ username }}</h5>
      <p>you can logout <a href="/logout">here</a>. </p>

      <h2>Websocket chat: type a message</h2>

	<form method="post" action="#" onSubmit="return Controller.onSubmit(this);">
	<table>
	<tr>
	<td>
		<div id="chatLog" class="" style="padding:40px 5px 5px 5px;border:1px solid #cdcdcd;overflow:scroll;height:320px;width:640px;"></div>
	</td>
	</tr>
	<tr>
	<td>
		<input type="text" class="text" id="chatMsg" />
	</td>
	</tr>
	<tr>
	<td>
		<input type="submit" value="Send Message" />
	</td>
	</tr>
	</table>
	</form>
  </div>
</div>

<!-- Footer -->
<div class="footer">
		<!--
        <ul class="icons">
				<li><a href="#" class="icon fab fa-twitter"><span class="label">Twitter</span></a></li>
				<li><a href="#" class="icon fab fa-facebook"><span class="label">Facebook</span></a></li>
				<li><a href="#" class="icon fab fa-google-plus"><span class="label">Google+</span></a></li>
				<li><a href="#" class="icon fab fa-pinterest"><span class="label">Pinterest</span></a></li>
				<li><a href="#" class="icon fab fa-dribbble"><span class="label">Dribbble</span></a></li>
				<li><a href="#" class="icon fab fa-linkedin"><span class="label">LinkedIn</span></a></li>
			</ul>
		-->
			<div class="copyright">
				<ul class="menu">
					<li>&copy; littlemole. All rights reserved.</li></li>
				</ul>
			</div>
</div>

<script src="/js/jquery.min.js"></script>
<script src="/js/jquery.cookie.js"></script>
<script src="/js/mvc.js"></script>
</body>
</html>

