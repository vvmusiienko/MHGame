game.loadMap("test3")

--~ local btn = game.addChild(GO_BUTTON, "ddbtn")
--~ btn.setSize(100, 40)
--~ btn.s_position(0, 40)
--~ btn.setNormalTexture("button.png")
--~ btn.setActiveTexture("button_a.png")
--~ btn.onClick = function(this)
--~   printf("Hello from lua!!!")
--~ end

--~ for i=0, 5, 1 do
--~   local label = game.addChild(GO_LABEL, string.format("lab%i", i))
--~   label.s_position(10,40 * i,0)
--~   label.setFont("scores_big")
--~   label.setText("0123456789 123712893712738712893")
--~   label.setFontSize(10.0 + i * 10)
--~ end


function aa()
printf("start")

for i=1, 250, 1 do
    local c = game.addChild(GO_CUBE, string.format("cube%i",i))
    c.s_size(100,10,30)
    c.setPosition(-200, 250 + i * 20, 0)
    c.setOrientation(0,0,0)
    c.setHidden(false)
    c.getConstraints()
    c.objectCreated()
    --c.setMass(0)
    makeLift(c, 200)
end


printf("end")
end



numb = 0

function itr(this)
  numb = numb + 0.5 * dt
  this.setText(tostring(math.ceil(numb)))
end

--game.registerIterator(label, itr)