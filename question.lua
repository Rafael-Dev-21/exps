--[[
-- What Kind of OOP is this shit?
-- I've seen this fucker on explanations on
--  how OOP used to work.
-- It's kinda composition oriented over inheritance oriented.
--]]--


function stackable(self)
  local stack = {}

  function self:empty()
    return #stack == 0
  end

  function self:push(ele)
    stack[#stack+1] = ele
  end

  function self:pop()
    local ele = stack[#stack]
    stack[#stack] = nil
    return ele
  end

  function self:size()
    return #stack
  end

  return self
end

function clearable(self)
  function self:clear()
    while not self:empty() do
      self:pop()
    end
  end

  return self
end

local stack = clearable(stackable({}))

stack:push(5)
stack:push(12)
stack:clear()
