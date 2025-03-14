#!/bin/bash

# Raspberry Pi Web Server Comprehensive Monitoring Script
# Monitors CPU, Memory, IO, Network and Temperature

# Setup monitoring parameters
MONITOR_INTERVAL=5  # Sampling interval (seconds)
LOG_DIR="server_monitor_$(date +%Y%m%d_%H%M%S)"  # Log directory
WEBSERVER_PROCESS="webserver"  # Web server process name, modify according to actual situation
DURATION=300  # Default monitoring duration (seconds), can be modified via parameters

# Parse command line arguments
while getopts "t:p:i:d:" opt; do
  case $opt in
    t) TEST_MODE=$OPTARG ;;    # Test mode: webbench, ab, or none
    p) WEBSERVER_PROCESS=$OPTARG ;;  # Process name
    i) MONITOR_INTERVAL=$OPTARG ;;   # Sampling interval
    d) DURATION=$OPTARG ;;     # Monitoring duration
    \?) echo "Invalid option: -$OPTARG" >&2; exit 1 ;;
  esac
done

# Create log directory
mkdir -p $LOG_DIR
echo "Monitoring logs will be saved in directory $LOG_DIR"

# Record system information
echo "==== System Information ====" > $LOG_DIR/system_info.log
date >> $LOG_DIR/system_info.log
uname -a >> $LOG_DIR/system_info.log
cat /proc/cpuinfo | grep Model >> $LOG_DIR/system_info.log
echo "Total Memory: $(free -h | grep Mem | awk '{print $2}')" >> $LOG_DIR/system_info.log
echo "==== Monitoring Started ====" >> $LOG_DIR/system_info.log
date >> $LOG_DIR/system_info.log

# Start CPU monitoring
monitor_cpu() {
  echo "Starting CPU monitoring..."
  (while true; do
    # Overall CPU usage
    CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | awk '{print $2 + $4}')
    
    # Specific process CPU usage
    if pgrep $WEBSERVER_PROCESS > /dev/null; then
      PROCESS_CPU=$(ps aux | grep $WEBSERVER_PROCESS | grep -v grep | awk '{print $3}')
    else
      PROCESS_CPU="Process not running"
    fi
    
    # Record CPU frequency
    CPU_FREQ=$(vcgencmd measure_clock arm | awk -F= '{printf "%.2f", $2 / 1000000000}')
    
    echo "$(date '+%Y-%m-%d %H:%M:%S') CPU_Usage=${CPU_USAGE}% Process_CPU=${PROCESS_CPU}% CPU_Freq=${CPU_FREQ}GHz" 
    sleep $MONITOR_INTERVAL
  done) > $LOG_DIR/cpu_usage.log &
  CPU_PID=$!
  echo $CPU_PID > $LOG_DIR/cpu_pid
}

# Start memory monitoring
monitor_memory() {
  echo "Starting memory monitoring..."
  (while true; do
    # Overall memory usage
    MEM_TOTAL=$(free -m | grep Mem | awk '{print $2}')
    MEM_USED=$(free -m | grep Mem | awk '{print $3}')
    MEM_USAGE_PCT=$(free | grep Mem | awk '{printf "%.2f", $3/$2 * 100}')
    
    # Specific process memory usage
    if pgrep $WEBSERVER_PROCESS > /dev/null; then
      PROCESS_MEM=$(ps aux | grep $WEBSERVER_PROCESS | grep -v grep | awk '{print $6/1024}')
    else
      PROCESS_MEM="Process not running"
    fi
    
    # Swap usage
    SWAP_USAGE=$(free -m | grep Swap | awk '{printf "%.2f", $3/$2 * 100}')
    
    echo "$(date '+%Y-%m-%d %H:%M:%S') Total=${MEM_TOTAL}MB Used=${MEM_USED}MB Usage=${MEM_USAGE_PCT}% Process_MEM=${PROCESS_MEM}MB Swap=${SWAP_USAGE}%"
    sleep $MONITOR_INTERVAL
  done) > $LOG_DIR/memory_usage.log &
  MEM_PID=$!
  echo $MEM_PID > $LOG_DIR/mem_pid
}

# Start IO monitoring
monitor_io() {
  echo "Starting IO monitoring..."
  (while true; do
    echo "===== $(date '+%Y-%m-%d %H:%M:%S') ====="
    iostat -x | grep -A 1 "Device"
    echo ""
    
    # Record process IO statistics
    if pgrep $WEBSERVER_PROCESS > /dev/null; then
      echo "Process IO statistics:"
      PID=$(pgrep $WEBSERVER_PROCESS)
      cat /proc/$PID/io 2>/dev/null || echo "Unable to read process IO information"
    fi
    sleep $MONITOR_INTERVAL
  done) > $LOG_DIR/io_usage.log &
  IO_PID=$!
  echo $IO_PID > $LOG_DIR/io_pid
}

# Start network monitoring
monitor_network() {
  echo "Starting network monitoring..."
  (while true; do
    echo "===== $(date '+%Y-%m-%d %H:%M:%S') ====="
    
    # Record network interface statistics
    IFACE=$(ip -o -4 route show to default | awk '{print $5}')
    RX_BYTES=$(cat /sys/class/net/$IFACE/statistics/rx_bytes)
    TX_BYTES=$(cat /sys/class/net/$IFACE/statistics/tx_bytes)
    
    # Wait for an interval to calculate rate
    sleep 1
    
    RX_BYTES_NEW=$(cat /sys/class/net/$IFACE/statistics/rx_bytes)
    TX_BYTES_NEW=$(cat /sys/class/net/$IFACE/statistics/tx_bytes)
    
    # Calculate bandwidth (KB/s)
    RX_RATE=$(( (RX_BYTES_NEW - RX_BYTES) / 1024 ))
    TX_RATE=$(( (TX_BYTES_NEW - TX_BYTES) / 1024 ))
    
    echo "Interface: $IFACE"
    echo "Download rate: ${RX_RATE} KB/s"
    echo "Upload rate: ${TX_RATE} KB/s"
    
    # Network connection status
    echo "Active connections:"
    netstat -ant | grep -E "ESTABLISHED|SYN_SENT|SYN_RECV" | wc -l
    
    # Listening ports
    echo "Listening ports:"
    netstat -ant | grep LISTEN
    
    # Wait for the remaining interval time
    sleep $(( MONITOR_INTERVAL - 1 ))
  done) > $LOG_DIR/network_usage.log &
  NET_PID=$!
  echo $NET_PID > $LOG_DIR/net_pid
}

# Start temperature monitoring
monitor_temperature() {
  echo "Starting temperature monitoring..."
  (while true; do
    TEMP=$(vcgencmd measure_temp | cut -d= -f2 | cut -d"'" -f1)
    THROTTLED=$(vcgencmd get_throttled)
    
    echo "$(date '+%Y-%m-%d %H:%M:%S') Temperature=${TEMP}°C Throttled=${THROTTLED}"
    sleep $MONITOR_INTERVAL
  done) > $LOG_DIR/temperature.log &
  TEMP_PID=$!
  echo $TEMP_PID > $LOG_DIR/temp_pid
}

# Start all monitoring
start_monitoring() {
  monitor_cpu
  monitor_memory
  monitor_io
  monitor_network
  monitor_temperature
  
  echo "All monitoring started"
}

# Stop all monitoring
stop_monitoring() {
  echo "Stopping all monitoring..."
  
  # Stop monitoring processes
  if [ -f $LOG_DIR/cpu_pid ]; then kill $(cat $LOG_DIR/cpu_pid) 2>/dev/null; fi
  if [ -f $LOG_DIR/mem_pid ]; then kill $(cat $LOG_DIR/mem_pid) 2>/dev/null; fi
  if [ -f $LOG_DIR/io_pid ]; then kill $(cat $LOG_DIR/io_pid) 2>/dev/null; fi
  if [ -f $LOG_DIR/net_pid ]; then kill $(cat $LOG_DIR/net_pid) 2>/dev/null; fi
  if [ -f $LOG_DIR/temp_pid ]; then kill $(cat $LOG_DIR/temp_pid) 2>/dev/null; fi
  
  echo "Monitoring stopped, logs saved in directory $LOG_DIR"
  
  # Generate simple statistics summary
  echo "Generating statistics summary..."
  
  echo "==== CPU Usage Statistics ====" > $LOG_DIR/summary.log
  grep CPU_Usage $LOG_DIR/cpu_usage.log | awk -F'CPU_Usage=' '{print $2}' | awk -F'%' '{print $1}' | awk '{ sum += $1; count++ } END { print "Average CPU usage: " sum/count "%" }'  >> $LOG_DIR/summary.log
  echo "Maximum CPU usage: $(grep CPU_Usage $LOG_DIR/cpu_usage.log | awk -F'CPU_Usage=' '{print $2}' | awk -F'%' '{print $1}' | sort -nr | head -1)%" >> $LOG_DIR/summary.log
  
  echo "==== Memory Usage Statistics ====" >> $LOG_DIR/summary.log
  grep Usage $LOG_DIR/memory_usage.log | awk -F'Usage=' '{print $2}' | awk -F'%' '{print $1}' | awk '{ sum += $1; count++ } END { print "Average memory usage: " sum/count "%" }' >> $LOG_DIR/summary.log
  echo "Maximum memory usage: $(grep Usage $LOG_DIR/memory_usage.log | awk -F'Usage=' '{print $2}' | awk -F'%' '{print $1}' | sort -nr | head -1)%" >> $LOG_DIR/summary.log
  
  echo "==== Temperature Statistics ====" >> $LOG_DIR/summary.log
  grep Temperature $LOG_DIR/temperature.log | awk -F'Temperature=' '{print $2}' | awk -F'°C' '{print $1}' | awk '{ sum += $1; count++ } END { print "Average temperature: " sum/count "°C" }' >> $LOG_DIR/summary.log
  echo "Maximum temperature: $(grep Temperature $LOG_DIR/temperature.log | awk -F'Temperature=' '{print $2}' | awk -F'°C' '{print $1}' | sort -nr | head -1)°C" >> $LOG_DIR/summary.log
  
  echo "Statistics summary saved to $LOG_DIR/summary.log"
}

# Run stress test
run_stress_test() {
  if [ "$TEST_MODE" = "webbench" ]; then
    echo "Running Webbench test..."
    which ./webbench > /dev/null || { echo "Webbench not installed, please install first"; exit 1; }
    
    # Default test parameters, can be adjusted as needed
    ./webbench -c 500 -t 30 http://localhost:9007/ > $LOG_DIR/webbench_results.log
    
  elif [ "$TEST_MODE" = "ab" ]; then
    echo "Running Apache Benchmark test..."
    which ab > /dev/null || { echo "Apache Benchmark not installed, please install first"; exit 1; }
    
    # Default test parameters, can be adjusted as needed
    ab -n 1000 -c 100 http://localhost:9007/ > $LOG_DIR/ab_results.log
  else
    echo "No test mode specified or using 'none' mode, monitoring only without stress testing"
    # If no test specified, wait for specified time before ending monitoring
    echo "Will monitor for $DURATION seconds before automatically stopping..."
    sleep $DURATION
  fi
}

# Main function
main() {
  echo "Starting Raspberry Pi web server monitoring..."
  
  # Start all monitoring
  start_monitoring
  
  # Run stress test (if specified)
  run_stress_test
  
  # Stop all monitoring
  stop_monitoring
  
  echo "Monitoring completed!"
}

# Capture CTRL+C and termination signals
trap stop_monitoring SIGINT SIGTERM

# Execute main function
main