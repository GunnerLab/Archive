#! /usr/bin/perl

$DIR=`pwd`;
@user= split(/\//,$DIR);
print "Hello $user[2], how are you today\n";

print "Please type in your pdb file name\n";
$input=<>;
print "thank you\n";
print "The pdb file you entered was $input\n";
chomp ($input);
`egrep MODEL $input > list`;
`sed /REMARK/d list >mlist`;

$ref= 'mlist';
open(P, $ref);
@plines=<P>;
foreach $ppl (@plines){
@last=split(/\s+/, $ppl);
$file=$last[0].$last[1].".pdb";
chop $ppl;
  
 `awk '/$ppl/','/ENDMDL/' $input > $file`;
 
}

close(P);
`rm list mlist`;

