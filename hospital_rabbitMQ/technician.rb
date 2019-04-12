require './employee'


class Technician < Employee

  private

  def initialize(skills: nil)
    super()

    @info_queue = @channel.queue('', exclusive: true)
    @info_queue.bind(@info_exchange)
    @info_queue.subscribe(block:false) do |_delivery_info, _properties, body|
      puts "#{body}"
    end

    @skills = skills
    puts "#{self}"

    # subscribing
    request_queues = @skills.map { |skill|
      skill_queue = @channel.queue("#{skill}")
      skill_queue.bind(@send_exchange, routing_key: "#{skill}")
      skill_queue
    }

    request_queues.each do |req_queue|
      req_queue.subscribe(block: false) do |delivery_info, _, body|
        @log_exchange.publish("#{@name} has received: #{body}")
        body_splitted = body.split("-")
        requesting_doctor = body_splitted[0]
        response_msg = "#{body_splitted[1]}  #{delivery_info.routing_key} - done"
        puts "Start doing sth"
        sleep(7)
        puts "End doing sth"
        @response_exchange.publish(response_msg, routing_key: requesting_doctor)
        @log_exchange.publish("#{@name} sending: #{response_msg}")
      end
    end
  end


  public

  def to_s
    "Technician: #{@name} , skills: #{@skills}"
  end

end

begin
  t = Technician.new(skills: ARGV)
  loop do

  end

rescue Interrupt => _e
  t.close
end
