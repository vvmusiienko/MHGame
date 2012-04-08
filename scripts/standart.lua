-- Variables
GO_CUBE		=			0
GO_GROUND	=			1
GO_LIGHT	=			2
GO_MODEL	=			3
GO_PLATE	=			4
GO_PLAYER	=			5
GO_CREATURE	=			6
GO_MAP_PROPS    =                       33
GO_BUTTON	=                       64
GO_LABEL        =                       65
GO_UNDEFINED    =                       255

GO_COMPOUND_CS  =			96
GO_BOX_CS	=			97
GO_SPHERE_CS	=		        98
GO_CYLINDER_CS	=		        99
GO_UNDEFINED_CS	=		        100
GO_CONE_CS	=			101
GO_CAPSULE_CS	=		        102
GO_MESH_CS	=			103

-- Constraints  =
GO_P2P_CONSTRAINT       =		128
GO_HINGE_CONSTRAINT	=	        129
GO_SLIDER_CONSTRAINT    =	        130


-- collision shape types
GOCST_COMPOUND_SHAPE = 0
GOCST_BOX = 1
GOCST_SPHERE = 2
GOCST_CYLINDER = 3



-- Creature states
CS_MOVE_RIGHT   =	        	0
CS_MOVE_LEFT	=               	1
CS_JUMP		=	        	2
CS_STAND	=		        3


-- array of "iterate" methods ([[id, iter],[id, iter]])
iters = {}


-- Functions
function printf(...)
  printInfo(string.format(...))
end

dofile(game.getResourcePath().."scripts/defaults.lua")
dofile(game.getResourcePath().."scripts/settings.lua")

function applyDefaults()
  for i=1,#defaults,1 do
    if not config.isExist(defaults[i].key) then
      if defaults[i].type == "integer" then
        config.setInt(defaults[i].key, defaults[i].value)
      end
      if defaults[i].type == "float" then
        config.setFloat(defaults[i].key, defaults[i].value)
      end
      if defaults[i].type == "string" then
        config.setString(defaults[i].key, defaults[i].value)
      end
      if defaults[i].type == "vector" then
        config.setVector(defaults[i].key, defaults[i].x, defaults[i].y, defaults[i].z)
      end
    end
  end
end


function applySettings()
  for i=1,#settings,1 do
    if settings[i].type == "integer" then
      config.setInt(settings[i].key, settings[i].value)
    end
    if settings[i].type == "float" then
      config.setFloat(settings[i].key, settings[i].value)
    end
    if settings[i].type == "string" then
      config.setString(settings[i].key, settings[i].value)
    end
    if settings[i].type == "vector" then
      config.setVector(settings[i].key, settings[i].x, settings[i].y, settings[i].z)
    end
  end
end


-- For safe execution some function. For case if function may by NIL
function se(fnc, ...)
  if fnc == nil then
    return
  end
  fnc(...)
end


function jt(t1, t2) -- join tables
  for k,v in ipairs(t2) do table.insert(t1, 1, v) end 
  return t1
end


function setCamera()
  local p = g_camera_position()
  local l = g_camera_lookAt()
  local u = g_camera_up()
  config.setVector("camera_position", p.x, p.y, p.z)
  config.setVector("camera_lookAt", l.x, l.y, l.z)
  config.setVector("camera_up", u.x, u.y, u.z)
end


function setPlayer()
  local p = g_camera_position()
  config.setVector("player_position", p.x, p.y, 0)
  mario.setPosition(p.x, p.y, 0)
end


game.registerIterator = function(obj, _iter)
  local pair = {id=obj, iter=_iter}
  table.insert(iters, pair)
end


game.unregisterIterator = function(obj)
    -- NEED TEST
    for i=1,#iters,1 do
      if iters[i].id == obj then
        table.remove(iters, i)
        return
      end
    end
end


function _cleanUp()
  map.onMapLoaded = nil

  local sub = 0
  for i=1,#iters,1 do
    if iters[i - sub].id.g_needDeath() then
      table.remove(iters, i - sub)
      sub = sub + 1
    end
  end
end


function _onGameInitialized()
end


function _onDraw2D()
  se(map.onDraw2D)
end


-- mouse buttons definition
L_B = 0
M_B = 1
R_B = 2


function _onMouseUp(mouseButton)
  se(map.onMouseUp, mouseButton)
end

function _onMouseDown(mouseButton)
  se(map.onMouseDown, mouseButton)
end

function _onMouseMoved(x, y)
  se(map.onMouseMoved,x,y)
end

function _onIterate(_dt)
  dt = _dt 
  if _dt > maxDT then
    maxDT = _dt;
  end
  --game.writeDebugText(string.format("Max DT : %f", maxDT))

  if not g_editorMode() then
    for i=1,#iters,1 do
      iters[i].iter(iters[i].id)
    end
  end
    
  if not g_editorMode() then
    se(map.onIterate, dt)
  end  
  
end


function _onMapWillLoad()
end



function _onMapLoaded()
  if not g_editorMode() then
    se(map.onMapLoaded)
  end
end

--------------------------- Constants
-- Contact Positions
CP_UP = 0
CP_DOWN = 1
CP_LEFT = 2
CP_RIGHT = 3


-- Initialize


maxDT = 0
dt = 1
applyDefaults()
applySettings()


function makeLift(obj, width)
  obj.startPos = obj.getPosition()
  obj.width = width
  obj.dx = -1
  obj.s_dx = 0.0 -- for smooth direction changing
  obj.setLinearFactor(1,0,0)
  obj.setAngularFactor(0,0,0)
  obj.onPhysicTick = function(this, dt)
    local p = obj.getPosition()
    local smooth_limit = (this.width/2) * 0.5;
    if p.x < this.startPos.x - this.width/2 then
      this.dx = 1
      this.s_dx = 0
    end
    if p.x > this.startPos.x + this.width/2 then
      this.dx = -1
      this.s_dx = 0
    end

    if math.abs(this.s_dx) < math.abs(this.dx) then
      this.s_dx = this.s_dx + this.dx / 35;
    end
    
    if this.dx > 0 then
      if p.x > this.startPos.x + smooth_limit then
        --local dif = (this.startPos.x + (this.width/2)) - (p.x + smooth_limit)
        local coef = 0.5--1.0 - math.abs(dif) / (smooth_limit)
        if coef < 0.1 then coef = 0.1 end
        this.s_dx = this.dx * coef
      end
    elseif p.x < this.startPos.x - smooth_limit then 
      if p.x < this.startPos.x - smooth_limit then
        --local dif = (this.startPos.x - (this.width/2)) - (p.x - smooth_limit)
        local coef = 0.5--1.0 - math.abs(dif) / (smooth_limit)
        if coef < 0.1 then coef = 0.1 end
        this.s_dx = this.dx * coef
      end
    end

    
    this.setLinearVelocity(this.s_dx, 0,0)
  end
  
  obj.onContactOccured = function(this, target, contactInfo)
  end
  
  obj.onContactLost = function(this, target, contactInfo)
  end
end


function makeSimpleBot(obj, dir, speed)
  
  obj.s_movingSpeed(speed);
  obj.dir = dir
  
  obj.onIterate = function(this)
    if this.dir then
      this.setState(CS_MOVE_RIGHT)
    else
      this.setState(CS_MOVE_LEFT)
    end
  end

   obj.onContactOccured = function(this, target, position)
    if position == CP_LEFT or  position == CP_RIGHT  then
      this.dir = not this.dir
    end
  end

  game.registerIterator(obj, obj.onIterate)
  
end



function makeAmbientBot(obj, radius)
  
  local startPosition = obj.getPosition()
  local lastTime = 0.0
  local nextTime = 0.0
  local isMovind = false
  local mx = 0.0
  local mz = 0.0
  
  
  obj.onIterate = function(this)
    lastTime = lastTime + dt
    if lastTime >= nextTime then
      lastTime = 0.0
      
      local ni = math.ceil(math.random() * 3)
      printf(ni)
      
      isMovind = false
      
      if ni == 1 then
        this.setFrameRate(0.5)
        obj.setAnim("stand", true)
        nextTime = 20.0
      end
      if ni == 2 then
        this.setFrameRate(0.3)
        obj.setAnim("stand2", false)
        nextTime = 30.0
      end
      if ni == 3 then
        this.setFrameRate(1.15)
        isMovind = true
        mx = 1.0 - math.random() * 2.0
        mz = 1.0 - math.random() * 2.0
        -- normalize
        s = math.sqrt(mx*mx + mz*mz)
	recip = 1.0/s
	mx = recip * mx
	mz = recip * mz
        
        nextTime = 30.0
        this.setOrientationM(mx, 0, mz)
        obj.setAnimImmediate("walk", true)
      end
    end
    
    if isMovind then
      local cp = this.getPosition()
      this.setPosition(cp.x + mx * dt, cp.y, cp.z + -mz * dt) 
    end
    
  end

  game.registerIterator(obj, obj.onIterate)
end







