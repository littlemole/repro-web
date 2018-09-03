<!DOCTYPE html>
<html>
<head>
<title>Hello Coro Websockets Registration</title>
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
    <h1>hello coroutine websockets registration</h1>
    <p>hello world websocket example registration with coroutines for modern reactive c++ for the web.</p>
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
      <h2>Hello Coroutine WebSockets Registration Example</h2>

				<h4>
					<h2>new user registration</h2>
				</h4>
				<p>please fill out all fields</p>
				<p><b style="color:red">{{errorMsg}}</b></p>
				<form action="/register" method="POST">
					<table style="width:50%;margin:5px auto;">
						<tr>
							<td>name</td>
							<td><input type="text" name="username"></td>
						</tr>
						<tr>
							<td>login</td>
							<td><input type="text" name="login"></td>
						</tr>
						<tr>
							<td>avatar_url</td>
							<td><input type="text" name="avatar_url" value=""></td>
						</tr>
						<tr>
							<td>password</td>
							<td><input type="password" name="pwd"></td>
						</tr>
						<tr>
							<td colspan=2><input type="submit" value="register"></td>
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

</body>
</html>


