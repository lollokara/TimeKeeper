$fn = 100;
screwhight = 6.2;
pcb_hight = 1.8;
back_thinckness = 2;
screw_size = 1.50;
screw_head = 3;


difference(){
    union(){
        difference(){
            cube([132,99+5,7.5]);
            translate([-1,0,7.5])
            rotate([-2.3,0,0])
            cube([134,100+5,6.5]);
            translate([2,7+5,0.99])
            linear_extrude(height = 2, center = true)
            scale([1.01,1.01,1])
            import (file = "TimeKeeper.dxf", layer = "Mechanical6");

            }
                // Spazi Viti
            translate([21.1+2,-2+7.5+5-1.15,1.5])
            cube([10,5,3.8],center = true);
            
            translate([107.1+2,-2+7.5+5-1.15,1.5])
            cube([10,5,3.8],center = true);
            
            translate([21.1+2,+7.5+86.5+5+0.5,1.5])
            cube([10,5,3.8],center = true);
            
            translate([107.1+2,+7.5+86.5+5+0.5,1.5])
            cube([10,5,3.8],center = true);
            
            difference(){
        translate([0, 0, 0])
        rotate(14.5, [1, 0, 0])
        cube([132,5,8]);
        translate([0, 0, -10])
        cube([132,99,20]);
        translate([0, -10, -8-5])
        cube([132,10,5]);
        translate([0, -10, 7.5])
        cube([132,10,5]);
        
        
    }
        }   
    //Fori Viti
    translate([21.1+2,+7+86.5+5,-5])
    cylinder(10,screw_size,screw_size);
    translate([21.1+2,+7+86.5+5,1])
    cylinder(8.6,screw_head,screw_head);
    translate([21.1+2,-2+7.5+5,-5])
    cylinder(10,screw_size,screw_size);
    translate([21.1+2,-2+7.5+5,1])
    cylinder(8.6,screw_head,screw_head);
    translate([107.1+2,-2+7.5+5,-5])
    cylinder(10,screw_size,screw_size);
    translate([107.1+2,-2+7.5+5,1])
    cylinder(8.6,screw_head,screw_head);
    translate([107.1+2,+7+86.5+5,-5])
    cylinder(10,screw_size,screw_size);
    translate([107.1+2,+7+86.5+5,1])
    cylinder(8.6,screw_head,screw_head);
    // Fori Display
    translate([2.6+1.2+(125/2),35.6+7.5+5,3])
    cube([125,15,6],center = true);
    translate([2.6+1.2+(125/2),68.1+7.5+5,3])
    cube([125,15,6],center = true);
    // Fori Pulsanti
    translate([2.6+30.78,7.5+5+10.45,1])
    cylinder(6.6,1.7,1.7);
    translate([2.6+30.78,7.5+5+10.45,1.98])
    cylinder(3.2,2.4,2.4);
    
    translate([2.6+39.38,7.5+5+10.45,1])
    cylinder(5.7,1.7,1.7);
    translate([2.6+39.38,7.5+5+10.45,1.98])
    cylinder(3.2,2.4,2.4);
    
    translate([2.6+47.99,7.5+5+10.45,1])
    cylinder(6.6,1.7,1.7);
    translate([2.6+47.99,7.5+5+10.45,1.98])
    cylinder(3.2,2.4,2.4);
    
    translate([2.6+95,7.5+5+8.7,1.98])
    cylinder(4.4,2.4,2.4);
    
    
    
}