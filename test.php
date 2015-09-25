<?php
  define('MAXSIZE', 200);

  function calcKmeans($file, $numClusters = 6, $use_kpp = true) { 
    $_img = imagecreatefromstring(file_get_contents($file));

    $img = imagecreatetruecolor(MAXSIZE, MAXSIZE * (imagesy($_img) / imagesx($_img) ));
    
    imagecopyresampled($img, 
     $_img, 
     0, 0, 0, 0, 
     MAXSIZE, 
     MAXSIZE * (imagesy($_img) / imagesx($_img) ),
     imagesx($_img), imagesy($_img)
    );

    $start = microtime();
    $clusters = kmeans($img, $numClusters, $use_kpp);
    $stop = microtime();
  
    uasort($clusters, function($a, $b) {
      $ac = $a['cnt'];
      $ab = $b['cnt'];
      if ($ac < $ab)
        return 1;
      else if ($ac > $ab)
        return -1;
      else
        return 0;
    });

    $total = 0;	
    foreach ($clusters as $c) {
      $total += $c['cnt'];
    }
    return [$total, $clusters, ($stop - $start) * 1000];
  }

 
  $sourceFiles = [
    'Ainon Lynx' => 'img/ainon.jpg',
    'The Fox' => 'img/the_fox.png',
    'Widecap mascot' => 'img/widecap.jpg',
    'Oskar' => 'img/oskar.jpg',
    'Buzludzha fursuiting' => 'img/buzludzha.jpg',
    'Firefox' => 'img/firefox.jpg'
  ];
?>
<!doctype html>
<html>
<head>
<meta charset="utf-8">
<title>k-means</title>
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css">
<script src="https://code.jquery.com/jquery-2.1.4.min.js"></script>
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js"></script>

<style>
.bottom {
    border-bottom: 1px solid #aaa;
}
</style>
<body>

<div class="container">

<div class="row">
  <h1>k-means clustering</h1>
  <p>This examples demonstrates finding of centroids for given images. </p>
</div>
    
<?
  foreach ($sourceFiles as $name => $file) { 
?>
<div class="row bottom">
  <div class="col-lg-6 col-md-6 col-sm-6 col-xs-6">
    <h3><?=$name?></h3>
    <img src="<?=$file;?>" width="400" height="auto" alt="<?=$name?>" title="<?=$name?>">
  </div>
  <div class="col-lg-6 col-md-6 col-sm-6 col-xs-6">
    <h3>Centroids</h3>
<?php  
    list($total, $clusters, $time) = calcKmeans($file, 6);
?>
   <table class="table table-striped table-bordered table-condensed">
   <thead>
   <tr>
     <th class="col-md-8">Color (cluster center)</th>
     <th class="col-md-2">Color (hex)</th>
     <th class="col-md-1">#Points</th>
     <th class="col-md-1">%</th>
   </tr>
   </thead>
<?php		
    foreach ($clusters as $c) { 
      $color = sprintf("%02x%02x%02x", $c['r'], $c['g'], $c['b']);
      if ($c['cnt'] > 0)
        echo "<tr><td><div style='background: #$color;width: 100%; height: 80px;'></div></td><td>#{$color}</td><td>".$c['cnt']."</td><td>".round(($c['cnt'] / $total) * 100, 1)."%</td></tr>\n";
    }
?>   
  </table>
  <h4>Calculation time: <?=$time?> msec</h4>
  </div>
</div>
 
<?php    
  }    
?>
</div>
</body>
</html>